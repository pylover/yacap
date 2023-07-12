// Copyright 2023 Vahid Mardani
/*
 * This file is part of CArg.
 *  CArg is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  CArg is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with CArg. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <clog.h>

#include "helpers.h"


struct carg_option nooption[] = {{ NULL }};
char out[BUFFSIZE + 1] = "\0";
char err[BUFFSIZE + 1] = "\0";


enum carg_status
carg_parse_string(struct carg *c, const char * line, void *userptr,
        void **handler) {
    char *argv[256];
    int argc = 0;
    char delim[1] = {' '};
    char *needle;
    char *saveptr = NULL;
    static char buff[BUFFSIZE + 1];
    strcpy(buff, line);

    needle = strtok_r(buff, delim, &saveptr);
    argv[argc++] = needle;
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        argv[argc++] = needle;
    }

    /* Piping */
    int outpipe[2];
    int errpipe[2];
    memset(out, 0, BUFFSIZE + 1);
    memset(err, 0, BUFFSIZE + 1);

    pipe(outpipe);
    pipe(errpipe);

    carg_outfile_set(outpipe[1]);
    carg_errfile_set(errpipe[1]);

    int ret = carg_parse(c, argc, (const char **)argv, userptr, handler);

    close(outpipe[1]);
    close(errpipe[1]);
    read(outpipe[0], out, BUFFSIZE);
    read(errpipe[0], err, BUFFSIZE);
    close(outpipe[0]);
    close(errpipe[0]);

    return ret;
}
