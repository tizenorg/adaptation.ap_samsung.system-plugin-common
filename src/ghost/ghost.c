/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/*
 * system-plugin-common
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "util.h"
#include "fileio.h"
#include "conf-parser.h"
#include "systemd.h"

#define GHOST_CONF              "/etc/ghost.conf"
#define GHOST_DIR               "/var/log/ghost"
#define GHOST_BOOT_DIR          GHOST_DIR "/boot"
#define GHOST_BOOTCOUNT         GHOST_BOOT_DIR "/bootcount"
#define GHOST_LINK_CURRENT      GHOST_BOOT_DIR "/current"

#define GHOST_CURRENT_BOOTTIME  GHOST_LINK_CURRENT "/boottime"
#define GHOST_CURRENT_PLOTCHART GHOST_LINK_CURRENT "/plotchart.svg"

#define GHOST_TIME_OUT         120

#define GHOST_THRESHOLD_DISK_USAGE_RATIO 3

int bootcount;
int ghost_mask;
bool arg_boottime = true;
bool arg_bootchart = false;
bool arg_plotchart = false;

enum GhostMask {
        MASK_BOOTTIME = 0,
        MASK_PLOTCHART,
        MASK_BOOTCHART,
};

static int parse_config_file(void) {
        const ConfigTableItem items[] = {
                { "Ghost",      "boottime",     config_parse_bool,      0,      &arg_boottime   },
                { "Ghost",      "plotchart",    config_parse_bool,      0,      &arg_plotchart  },
                { "Ghost",      "bootchart",    config_parse_bool,      0,      &arg_bootchart  },
                { NULL,         NULL,           NULL,                   0,      NULL            }
        };

        int r;

        r = config_parse(GHOST_CONF, (void*) items);
        if (r < 0) {
                fprintf(stderr, "Failed to parse configuration file: %s\n", strerror(-r));
                return r;
        }

        return 0;
}

static int get_bootcount_from_file(int *count) {
        _cleanup_free_ char *s = NULL;
        int r;

        r = read_one_line_file(GHOST_BOOTCOUNT, &s);
        if (r < 0 && errno != ENOENT)
                return r;

        *count = (s) ? atoi(s) : 0;
        return 0;
}

static int get_bootcount_from_dir(int *count) {
        _cleanup_closedir_ DIR *dir = NULL;
        struct dirent *de;
        int c = 0;

        dir = opendir(GHOST_BOOT_DIR);
        if (!dir)
                return errno;

        FOREACH_DIRENT(de, dir, return -errno) {
                if (de->d_type != DT_DIR)
                        continue;

                c = atoi(de->d_name) > c ? atoi(de->d_name) : c;
        }

        *count = c;
        return 0;
}

static int update_bootcount(void) {
        _cleanup_free_ char *count = NULL;
        int r, from_dir = 0, from_file = 0;

        r = get_bootcount_from_file(&from_file);
        if (r < 0)
                return r;

        r = get_bootcount_from_dir(&from_dir);
        if (r < 0)
                return r;

        bootcount = from_file > from_dir ? from_file : from_dir;

        r = asprintf(&count, "%d", ++bootcount);
        if (r < 0)
                return r;

        r = write_string_file(GHOST_BOOTCOUNT, count);
        if (r < 0)
                return r;

        return 0;
}


struct df_info {
        char device[30];
        int blocks;
        int used;
        int available;
        int use_ratio;
        char mount_on[PATH_MAX];
};

#define GHOST_CURRENT_DISKINFO GHOST_LINK_CURRENT "/diskinfo"

static void ghost_parse_df_info(struct df_info *info, char *buf) {
        char *p = buf;
        char t[30];
        size_t len;

        /* Filesystem */
        len = strcspn(p, WHITESPACE);
        snprintf(info->device, len+1, "%s", p);
        p += len + strspn(p+len, WHITESPACE);

        /* 1K-blocks */
        len = strcspn(p, WHITESPACE);
        snprintf(t, len+1, "%s", p);
        info->blocks = atoi(t);
        p += len + strspn(p+len, WHITESPACE);

        /* Used */
        len = strcspn(p, WHITESPACE);
        snprintf(t, len+1, "%s", p);
        info->used = atoi(t);
        p += len + strspn(p+len, WHITESPACE);

        /* Available */
        len = strcspn(p, WHITESPACE);
        snprintf(t, len+1, "%s", p);
        info->available = atoi(t);
        p += len + strspn(p+len, WHITESPACE);

        /* Use% */
        len = strcspn(p, WHITESPACE);
        snprintf(t, len, "%s", p);
        info->use_ratio = atoi(t);
        p += len + strspn(p+len, WHITESPACE);

        /* Mounted on */
        len = strcspn(p, NEWLINE);
        snprintf(info->mount_on, len+1, "%s", p);
}

static int ghost_get_disk_info(struct df_info *info) {
        int pipefd[2];
        int status;
        int r;

        pipe(pipefd);
        if(fork() == 0) {
                char * const args[] = {"/bin/df", GHOST_DIR, NULL};

                close(pipefd[0]);
                dup2(pipefd[1], 1);
                dup2(pipefd[1], 2);
                usleep(1000);
                execvp(args[0], args);

                close(pipefd[1]);
                _exit(1);
        } else {
                char buf[LINE_MAX*2];
                char *p = buf;

                close(pipefd[1]);
                wait(&status);
                if (!WIFEXITED(status))
                        return -WEXITSTATUS(status);

                /* size of buf maybe bigger than df output */
                r = read(pipefd[0], buf, sizeof(buf));
                if (r < 0)
                        return -errno;

                /* discard first line */
                p += strcspn(p, NEWLINE)+1;
                truncate_nl(p);

                ghost_parse_df_info(info, p);
        }

        return 0;
}

static int ghost_get_disk_usage(int *usage) {
        int pipefd[2];
        int status;
        int r;

        pipe(pipefd);
        if(fork() == 0) {
                char * const args[] = {"/usr/bin/du", "-s", GHOST_DIR, NULL};

                close(pipefd[0]);
                dup2(pipefd[1], 1);
                dup2(pipefd[1], 2);
                usleep(1000);
                execvp(args[0], args);

                close(pipefd[1]);
                _exit(1);
        } else {
                char buf[LINE_MAX];

                close(pipefd[1]);
                wait(&status);
                if (!WIFEXITED(status))
                        return -WEXITSTATUS(status);

                /* size of buf maybe bigger than du output */
                r = read(pipefd[0], buf, sizeof(buf));
                if (r < 0)
                        return -errno;

                buf[strcspn(buf, WHITESPACE)] = 0;
                *usage = atoi(buf);
        }

        return 0;
}

static gint sort_num_str_by_ascending(gconstpointer a, gconstpointer b) {
        return atoi(a) - atoi(b);
}

static int generate_dir_list(GList **list, DIR *dir) {
        struct dirent *de;
        GList *l = NULL;

        FOREACH_DIRENT(de, dir, goto out) {
                if (de->d_type != DT_DIR)
                        continue;

                l = g_list_prepend(l, de->d_name);
        }

        l = g_list_sort(l, sort_num_str_by_ascending);
        *list = l;
        return 0;

out:
        if (l)
            g_list_free(l);
        return -errno;
}

static int check_threshhold(void) {
        _cleanup_closedir_ DIR *dir = NULL;
        _cleanup_free_ struct df_info *df = NULL;
        int r, usage;
        GList *list = NULL, *head = NULL;

        df = new0(struct df_info, 1);
        r = ghost_get_disk_info(df);
        if (r < 0) {
                fprintf(stderr, "Failed to get disk info\n");
                return r;
        }

        r = ghost_get_disk_usage(&usage);
        if (r < 0) {
                fprintf(stderr, "Failed to get disk usage\n");
                return r;
        }

        if ((usage * 100 / df->blocks) < GHOST_THRESHOLD_DISK_USAGE_RATIO)
                return 0;

        dir = opendir(GHOST_BOOT_DIR);
        if (!dir) {
                fprintf(stderr, "Failed to open dir: %s", GHOST_BOOT_DIR);
                return -errno;
        }

        r = generate_dir_list(&list, dir);
        if (r < 0)
                goto finish;

        do {
                _cleanup_free_ char *path = NULL;

                head = g_list_first(list);
                r = asprintf(&path, "%s/%s", GHOST_BOOT_DIR, (char *)head->data);
                if (r < 0)
                        goto finish;

                r = rmdir_recursive(path);
                if (r < 0) {
                        fprintf(stderr, "Failed to delete directory: %s\n", dir);
                        r = -errno;
                        goto finish;
                }

                list = g_list_remove(list, list->data);

                r = ghost_get_disk_usage(&usage);
                if (r < 0) {
                        fprintf(stderr, "Failed to get disk usage\n");
                        goto finish;
                }
        } while((usage * 100 / df->blocks) >= GHOST_THRESHOLD_DISK_USAGE_RATIO);

        r = 0;
finish:
        g_list_free(list);
        return r;
}

static int prepare_working_dir(void) {
        _cleanup_free_ char *path = NULL, *count = NULL;
        int r;

        r = asprintf(&path, "%s/%d", GHOST_BOOT_DIR, bootcount);
        if (r < 0)
                return r;

        r = mkdir(path, 0755);
        if (r < 0)
                return -errno;

        r = unlink(GHOST_LINK_CURRENT);
        if (r < 0 && errno != ENOENT)
                return -errno;

        r = asprintf(&count, "%d", bootcount);
        if (r < 0)
                return r;

        r = symlink(count, GHOST_LINK_CURRENT);
        if (r < 0)
                return -errno;

        return 0;
}

static int set_loopmask(void) {
        int mask = 0;

        return mask =
                (arg_boottime ? (0x01) << MASK_BOOTTIME : 0x00) |
                (arg_plotchart ? (0x01) << MASK_PLOTCHART : 0x00) |
                (arg_bootchart ? (0x01) << MASK_BOOTCHART : 0x00);
}

static int get_boottime_checkpoint(void) {
        _cleanup_free_ char *result = NULL, *active_state = NULL, *platform_sec = NULL;
        unsigned int exec_main_pid = 0, main_pid = 0;
        unsigned long long inactive_enter_timestamp_monotonic, exec_main_exit_timestamp_monotonic;
        int r, exec_main_code = 0;

        if (!(ghost_mask & 0x01 << MASK_BOOTTIME))
                return 0;

        r = systemd_get_unit_property_as_string("boot-animation.service",
                                                "ActiveState",
                                                &active_state);
        if (r < 0)
                return -r;

        r = systemd_get_unit_property_as_uint64("boot-animation.service",
                                                "InactiveEnterTimestampMonotonic",
                                                &inactive_enter_timestamp_monotonic);
        if (r < 0)
                return -r;

        r = systemd_get_service_property_as_string("boot-animation.service",
                                                   "Result",
                                                   &result);
        if (r < 0)
                return -r;

        if (streq(active_state, "inactive") &&
            inactive_enter_timestamp_monotonic != 0 &&
            streq(result, "success")) {
                ghost_mask &= ~(0x01 << MASK_BOOTTIME);
                r = systemd_get_service_property_as_uint64("boot-animation.service",
                                                           "ExecMainExitTimestampMonotonic",
                                                           &exec_main_exit_timestamp_monotonic);
                if (r < 0)
                        return -r;

                r = asprintf(&platform_sec, "platform(sec): %.2lf",
                             ((float)(exec_main_exit_timestamp_monotonic/1000)/1000));
                if (r < 0)
                        return r;

                r = write_string_file(GHOST_CURRENT_BOOTTIME, platform_sec);
                if (r < 0)
                        return r;
        }

        return 0;
}

static int ghost_processing_plotchart(void) {
        _cleanup_free_ char *active_state = NULL;
        pid_t my_pid, parent_pid, child_pid;
        int r, status;
        unsigned int jobs;

        if (!(ghost_mask & 0x01 << MASK_PLOTCHART))
                return 0;

        r = systemd_get_unit_property_as_string("default.target",
                                                "ActiveState",
                                                &active_state);
        if (r < 0)
                return -r;

        if (!streq(active_state, "active"))
                return 0;

        r = systemd_get_manager_property_as_uint32(DBUS_INTERFACE_SYSTEMD_MANAGER,
                                                   "NJobs",
                                                   &jobs);
        if (r < 0)
                return -r;

        if (jobs)
                return 0;

        if((child_pid = fork()) < 0 ) {
                perror("fork failure");
                exit(1);
        }

        if(child_pid == 0) {
                char * const args[] = {"/usr/bin/systemd-analyze", "plot", NULL};
                int fd = open(GHOST_CURRENT_PLOTCHART, O_RDWR | O_CREAT, 0644);

                if (fd < 0)
                        _exit(EXIT_FAILURE);

                dup2(fd, 1);
                execvp(args[0], args);

                close(fd);
                _exit(1);
        } else {
                wait(&status);

                if (WIFEXITED(status))
                        ghost_mask &= ~(0x01 << MASK_PLOTCHART);
        }
        return 0;
}

static bool is_bootchart_set_as_init(void) {
        _cleanup_free_ char *cmdline  = NULL;
        char *w, *state;
        size_t l;
        bool contained = false;
        int r;

        if (read_one_line_file("/proc/cmdline", &cmdline) < 0) {
                fprintf(stderr, "Failed to read /proc/cmdline.\n");
                return false;
        }

        FOREACH_WORD(w, l, cmdline, state)
                if (strneq("init=/usr/lib/systemd/systemd-bootchart", w, l))
                        return true;

        return false;
}

static int ghost_processing_bootchart(pid_t bootchart_pid) {
        _cleanup_free_ char *proc_pid = NULL, *bootchart_path = NULL;
        _cleanup_closedir_ DIR *dir = NULL;
        struct dirent *de;
        int r;

        if (!(ghost_mask & 0x01 << MASK_BOOTCHART))
                return 0;

        /* If bootchart_pid is 0, then maybe ghost is started after
         * bootchart had finished. So safe and goto copy. */
        if (bootchart_pid == 0)
                goto copy_bootchart;

        r = asprintf(&proc_pid, "/proc/%d", bootchart_pid);
        if (r < 0)
                return r;

        /* Do NOT be confused for the return check. We hope the
         * bootchart is finished to get the result. So its process id
         * should be disappeared. */
        r = access(proc_pid, F_OK);
        if (r == 0)
                return 0;

        if (r < 0 &&
            errno != ENOENT)
                return -errno;

copy_bootchart:
        dir = opendir("/run/log");
        if (!dir)
                return -errno;

        FOREACH_DIRENT(de, dir, return -errno) {
                if (de->d_type != DT_REG)
                        continue;

                if (startswith(de->d_name, "bootchart-")) {
                        r = asprintf(&bootchart_path, "/run/log/%s", de->d_name);
                        if (r < 0)
                                continue;

                        r = do_copy(bootchart_path, GHOST_LINK_CURRENT, NULL);
                        if (r < 0) {
                                fprintf(stderr, "Failed to copy bootchart.\n");
                                continue;
                        }
                }
        }

        ghost_mask &= ~(0x01 << MASK_BOOTCHART);
        return 0;
}

static int ghost_loop(void) {
        int sec = 0;
        int r;
        bool do_bootchart;
        pid_t pid_of_bootchart;

        do_bootchart = is_bootchart_set_as_init();
        if (do_bootchart)
                pid_of_bootchart = pid_of("systemd-bootchart");
        else
                ghost_mask &= ~(0x01 << MASK_BOOTCHART);

        while (ghost_mask) {
                /* check boot-animation finished */
                r = get_boottime_checkpoint();
                if (r < 0)
                        return r;

                /* check booting completed(default.target) */
                r = ghost_processing_plotchart();
                if (r < 0)
                        return r;

                /* If init=/usr/lib/systemd/systemd-bootchar is
                 * contained at kernel command line then the bootchart
                 * will be copied to ghost current working
                 * directory. */
                if(do_bootchart) {
                        r = ghost_processing_bootchart(pid_of_bootchart);
                        if (r < 0)
                                return r;
                }

                sleep(1);
                if (sec++ > GHOST_TIME_OUT) {
                        fprintf(stderr, "ghost loop time(almost %dsec) over.\n",
                                GHOST_TIME_OUT);
                        break;
                }
        }

        return 0;
}

int main(int argc, char *argv[]) {
        int r;

        r = parse_config_file();
        if (r < 0)
                return r;

        if (do_mkdir(GHOST_BOOT_DIR, 0755) && errno != EEXIST) {
                fprintf(stderr, "Failed to create ghost directory: %s\n", GHOST_BOOT_DIR);
                return -errno;
        }

        /* boot count update */
        r = update_bootcount();
        if (r < 0) {
                fprintf(stderr, "Failed to update bootcount: %s\n", strerror(-r));
                return r;
        } else
                fprintf(stdout, "[Ghost] Now, %d%s boot.\n",
                        bootcount,
                        bootcount == 1 ? "st" :
                        bootcount == 2 ? "nd" :
                        bootcount == 3 ? "rd" : "th");

        r = check_threshhold();
        if (r < 0) {
                fprintf(stderr, "Failed to check threshold: %s\n", strerror(-r));
                return r;
        }

        r = prepare_working_dir();
        if (r < 0) {
                fprintf(stderr, "Failed to make current boot dir: %s\n", strerror(-r));
                return r;
        }

        ghost_mask = set_loopmask();
        r = ghost_loop();
        if (r < 0)
                return r;

        return 0;
}
