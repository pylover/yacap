// Copyright 2023 Vahid Mardani
/*
 * This file is part of Carrow.
 *  Carrow is free software: you can redistribute it and/or modify it under 
 *  the terms of the GNU General Public License as published by the Free 
 *  Software Foundation, either version 3 of the License, or (at your option) 
 *  any later version.
 *  
 *  Carrow is distributed in the hope that it will be useful, but WITHOUT ANY 
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 *  details.
 *  
 *  You should have received a copy of the GNU General Public License along 
 *  with Carrow. If not, see <https://www.gnu.org/licenses/>. 
 *  
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <clog.h>

#include "carg.h"
#include "print.h"


static int _outfile = STDOUT_FILENO;
static int _errfile = STDERR_FILENO;


void
carg_outfile_set(int fd) {
    _outfile = fd;
}


void
carg_errfile_set(int fd) {
    _errfile = fd;
}


void
carg_print_usage(struct carg_state *state) {
    char delim[1] = {'\n'};
    char *needle;
    char *saveptr = NULL;

    dprintf(state->fd, "Usage: %s [OPTION...]", state->argv[0]);
    if (state->carg->args == NULL) {
        goto done;
    }

    static char buff[1024];
    strcpy(buff, state->carg->args);

    needle = strtok_r(buff, delim, &saveptr);
    dprintf(state->fd, " %s", needle);
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        dprintf(state->fd, "\n   or: %s [OPTION...] %s", state->argv[0], needle);
    }

done:
    dprintf(state->fd, "\n");
}


void
carg_print_help(struct carg_state *state) {
    /* Usage */
    carg_print_usage(state);

    /* Document */
    print_multiline(state->fd, state->carg->doc, 0, HELP_LINESIZE);

    /* Options */
    print_options(state->fd, state->carg);

    /* Footer */
    print_multiline(state->fd, state->carg->footer, 0, HELP_LINESIZE);
}


static int
carg_parseopt(struct carg_state *state) {
}


int
carg_parse(struct carg *c, int argc, char **argv) {
    if (argc < 1) {
        return -1;
    }

    struct carg_state state = {
        .carg = c,
        .argc = argc,
        .argv = argv,
        .fd = _outfile,
    };

    carg_print_help(&state);

    return 1;
}
