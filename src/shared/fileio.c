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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "fileio.h"
#include "util.h"

int write_string_to_file(FILE *f, const char *line) {
        errno = 0;
        fputs(line, f);
        if (!endswith(line, "\n"))
                fputc('\n', f);

        fflush(f);

        if (ferror(f))
                return errno ? -errno : -EIO;

        return 0;
}

int write_string_file(const char *fn, const char *line) {
        _cleanup_fclose_ FILE *f = NULL;

        assert(fn);
        assert(line);

        f = fopen(fn, "we");
        if (!f)
                return -errno;

        return write_string_to_file(f, line);
}

int read_one_line_from_file(FILE *f, char **line) {
        char t[LINE_MAX], *c;

        errno = 0;

        if (!fgets(t, sizeof(t), f)) {

                if (ferror(f))
                        return errno ? -errno : -EIO;

                t[0] = 0;
        }

        c = strdup(t);
        if (!c)
                return -ENOMEM;

        *line = truncate_nl(c);
        return 0;
}

int read_one_line_file(const char *fn, char **line) {
        _cleanup_fclose_ FILE *f = NULL;
        char t[LINE_MAX], *c;

        assert(fn);
        assert(line);

        f = fopen(fn, "re");
        if (!f)
                return -errno;

        return read_one_line_from_file(f, line);
}
