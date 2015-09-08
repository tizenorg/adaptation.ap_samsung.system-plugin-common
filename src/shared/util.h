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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#include "macro.h"

#define WHITESPACE " \t\n\r"
#define NEWLINE "\n\r"
#define QUOTES "\"\'"
#define COMMENTS "#;"

static inline void freep(void *p) {
        free(*(void**) p);
}

static inline void fclosep(FILE **f) {
        if (*f)
                fclose(*f);
}

static inline void closedirp(DIR **d) {
        if (*d)
                closedir(*d);
}

static inline const char *startswith(const char *s, const char *prefix) {
        if (strncmp(s, prefix, strlen(prefix)) == 0)
                return s + strlen(prefix);
        return NULL;
}

#define _cleanup_free_ _cleanup_(freep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)

#define streq(a,b) (strcmp((a),(b)) == 0)
#define strneq(a, b, n) (strncmp((a), (b), (n)) == 0)
#define strcaseeq(a,b) (strcasecmp((a),(b)) == 0)
#define strncaseeq(a, b, n) (strncasecmp((a), (b), (n)) == 0)

#define new(t, n) ((t*) malloc(sizeof(t) * (n)))
#define new0(t, n) ((t*) calloc((n), sizeof(t)))
#define malloc0(n) (calloc((n), 1))

#define NULSTR_FOREACH(i, l)                                    \
        for ((i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

bool streq_ptr(const char *a, const char *b) _pure_;
int parse_boolean(const char *v) _pure_;
char *truncate_nl(char *s);
char *strnappend(const char *s, const char *suffix, size_t b);
char *strappend(const char *s, const char *suffix);
char *strstrip(char *s);
char *file_in_same_dir(const char *path, const char *filename);
bool nulstr_contains(const char*nulstr, const char *needle);
bool path_is_absolute(const char *p);
char *path_kill_slashes(char *path);
char* endswith(const char *s, const char *postfix);

char *split(const char *c, size_t *l, const char *separator, char **state);

#define FOREACH_WORD(word, length, s, state)                            \
        for ((state) = NULL, (word) = split((s), &(length), WHITESPACE, &(state)); (word); (word) = split((s), &(length), WHITESPACE, &(state)))

#define FOREACH_WORD_SEPARATOR(word, length, s, separator, state)       \
        for ((state) = NULL, (word) = split((s), &(length), (separator), &(state)); (word); (word) = split((s), &(length), (separator), &(state)))

#define FOREACH_DIRENT(de, d, on_error)                                 \
        for (errno = 0, de = readdir(d);; errno = 0, de = readdir(d))   \
                if (!de) {                                              \
                        if (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        break;                                          \
                } else if (streq(de->d_name, ".") ||                    \
                           streq(de->d_name, ".."))                     \
                        continue;                                       \
                else

bool is_number(const char *s, int l);
int pid_of(const char *pname);
int do_copy(const char *src, const char *dst, const char *option);
int do_mkdir_one(const char *path, mode_t mode);
int do_mkdir(const char *path, mode_t mode);
int rmdir_recursive(const char *path);
