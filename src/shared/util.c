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
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "fileio.h"
#include "util.h"

/* In old kernel, this symbol maybe NOT */
#ifndef TASK_COMM_LEN
#define TASK_COMM_LEN 16
#endif

bool streq_ptr(const char *a, const char *b) {

        /* Like streq(), but tries to make sense of NULL pointers */

        if (a && b)
                return streq(a, b);

        if (!a && !b)
                return true;

        return false;
}

char *truncate_nl(char *s) {
        assert(s);

        s[strcspn(s, NEWLINE)] = 0;

        return s;
}

char *strnappend(const char *s, const char *suffix, size_t b) {
        size_t a;
        char *r;

        if (!s && !suffix)
                return strdup("");

        if (!s)
                return strndup(suffix, b);

        if (!suffix)
                return strdup(s);

        assert(s);
        assert(suffix);

        a = strlen(s);
        if (b > ((size_t) -1) - a)
                return NULL;

        r = new(char, a+b+1);
        if (!r)
                return NULL;

        memcpy(r, s, a);
        memcpy(r+a, suffix, b);
        r[a+b] = 0;

        return r;
}

char *strappend(const char *s, const char *suffix) {
        return strnappend(s, suffix, suffix ? strlen(suffix) : 0);
}

char *strstrip(char *s) {
        char *e;

        /* Drops trailing whitespace. Modifies the string in
         * place. Returns pointer to first non-space character */

        s += strspn(s, WHITESPACE);

        for (e = strchr(s, 0); e > s; e --)
                if (!strchr(WHITESPACE, e[-1]))
                        break;

        *e = 0;

        return s;
}

char *file_in_same_dir(const char *path, const char *filename) {
        char *e, *r;
        size_t k;

        assert(path);
        assert(filename);

        /* This removes the last component of path and appends
         * filename, unless the latter is absolute anyway or the
         * former isn't */

        if (path_is_absolute(filename))
                return strdup(filename);

        if (!(e = strrchr(path, '/')))
                return strdup(filename);

        k = strlen(filename);
        if (!(r = new(char, e-path+1+k+1)))
                return NULL;

        memcpy(r, path, e-path+1);
        memcpy(r+(e-path)+1, filename, k+1);

        return r;
}

bool nulstr_contains(const char*nulstr, const char *needle) {
        const char *i;

        if (!nulstr)
                return false;

        NULSTR_FOREACH(i, nulstr)
                if (streq(i, needle))
                        return true;

        return false;
}

bool path_is_absolute(const char *p) {
        return p[0] == '/';
}

char *path_kill_slashes(char *path) {
        char *f, *t;
        bool slash = false;

        /* Removes redundant inner and trailing slashes. Modifies the
         * passed string in-place.
         *
         * ///foo///bar/ becomes /foo/bar
         */

        for (f = path, t = path; *f; f++) {

                if (*f == '/') {
                        slash = true;
                        continue;
                }

                if (slash) {
                        slash = false;
                        *(t++) = '/';
                }

                *(t++) = *f;
        }

        /* Special rule, if we are talking of the root directory, a
           trailing slash is good */

        if (t == path && slash)
                *(t++) = '/';

        *t = 0;
        return path;
}

char* endswith(const char *s, const char *postfix) {
        size_t sl, pl;

        assert(s);
        assert(postfix);

        sl = strlen(s);
        pl = strlen(postfix);

        if (pl == 0)
                return (char*) s + sl;

        if (sl < pl)
                return NULL;

        if (memcmp(s + sl - pl, postfix, pl) != 0)
                return NULL;

        return (char*) s + sl - pl;
}

int parse_boolean(const char *v) {
        assert(v);

        if (streq(v, "1") || v[0] == 'y' || v[0] == 'Y' || v[0] == 't' || v[0] == 'T' || strcaseeq(v, "on"))
                return 1;
        else if (streq(v, "0") || v[0] == 'n' || v[0] == 'N' || v[0] == 'f' || v[0] == 'F' || strcaseeq(v, "off"))
                return 0;

        return -EINVAL;
}

/* Split a string into words. */
char *split(const char *c, size_t *l, const char *separator, char **state) {
        char *current;

        current = *state ? *state : (char*) c;

        if (!*current || *c == 0)
                return NULL;

        current += strspn(current, separator);
        *l = strcspn(current, separator);
        *state = current+*l;

        return (char*) current;
}

bool is_number(const char *s, int l) {
        int i;

        for (i = 0; i < l; i++)
                if (!isdigit(s[i]))
                        return false;

        return true;
}

int pid_of(const char *pname) {
        _cleanup_closedir_ DIR *dir = NULL;
        struct dirent *de;
        int r;

        dir = opendir("/proc");
        if (!dir) {
                fprintf(stderr, "Failed to open dir: %s", "/proc");
                return -errno;
        }

        FOREACH_DIRENT(de, dir, return -errno) {
                _cleanup_free_ char *path = NULL;
                _cleanup_free_ char *comm = NULL;

                if (de->d_type != DT_DIR)
                        continue;

                if (!is_number(de->d_name, strlen(de->d_name)))
                        continue;

                r = asprintf(&path, "/proc/%s/comm", de->d_name);
                if (r < 0)
                        return -ENOMEM;

                r = read_one_line_file(path, &comm);
                if (r < 0)
                        continue;

                if (strneq(pname, comm, TASK_COMM_LEN-1))
                        return atoi(de->d_name);
        }

        return 0;
}

int do_copy(const char *src, const char *dst, const char *option) {
        pid_t pid;
        int status;

        pid = fork();
        if (pid == 0) { /* child */
                execl("/bin/cp",
                      "/bin/cp", src,  dst, option, (char *)0);
                _exit(1);
        }
        else if (pid < 0)
                return -errno;
        /* TODO */
        /* Child wait status should be checked. */
        wait(&status);
        return 0;
}

int do_mkdir_one(const char *path, mode_t mode) {
        int r;

        assert(path);

        r = mkdir(path, mode);
        if (r < 0) {
                fprintf(stderr, "cannot create directory '%s': %m\n", path);
                return r;
        }

        return 0;
}

int do_mkdir(const char *path, mode_t mode) {
        size_t s, l;
        int p;
        int r;

        assert(path);

        l = strlen(path);

        for (p = 0, s = 0; p < l; p += s+1) {
                _cleanup_free_ char *d = new0(char, p+s+1);

                s = strcspn(path+p, "/");
                if (!s)
                        continue;

                r = snprintf(d, p+s+1, "%s", path);
                if (r < 0)
                        return r;

                r = access(d, W_OK);
                if (r == 0)
                        continue;
                else if (r < 0 && errno != ENOENT) {
                        fprintf(stderr,
                                "cannot create directory '%s': %m\n", d);
                        return r;
                }

                r = do_mkdir_one(d, mode);
                if (r < 0)
                        return r;
        }

        return 0;
}

int rmdir_recursive(const char *path) {
        _cleanup_closedir_ DIR *d = NULL;
        struct dirent *de;
        int r;

        assert(path);

        d = opendir(path);
        if (!d)
                return -errno;

        FOREACH_DIRENT(de, d, return -errno) {
                _cleanup_free_ char *p = NULL;

                r = asprintf(&p, "%s/%s", path, de->d_name);
                if (r < 0)
                        return -ENOMEM;

                if (de->d_type == DT_DIR) {
                        r = rmdir_recursive(p);
                        if (r < 0)
                                return r;
                } else {
                        r = unlink(p);
                        if (r < 0)
                                return r;
                }
        }

        return rmdir(path);
}
