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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "help.h"
#include "state.h"


void
carg_usage_print(const struct carg *c) {
    char delim[1] = {'\n'};
    char *needle;
    char *saveptr = NULL;
    struct carg_state *state = c->state;

    dprintf(STDOUT_FILENO, "Usage: %s [OPTION...]", state->prog);
    if (c->args == NULL) {
        goto done;
    }

    char *buff = malloc(strlen(c->args) + 1);
    strcpy(buff, c->args);

    needle = strtok_r(buff, delim, &saveptr);
    dprintf(STDOUT_FILENO, " %s", needle);
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        dprintf(STDOUT_FILENO, "\n   or: %s [OPTION...] %s", state->prog,
                needle);
    }

done:
    free(buff);
    dprintf(STDOUT_FILENO, "\n");
}


void
carg_help_print(const struct carg *c) {
    // /* Usage */
    carg_usage_print(c);

    // /* Header */
    // if (state->carg->header) {
    //     dprintf(state->fd, "\n");
    //     _print_multiline(state->fd, state->carg->header, 0, HELP_LINESIZE);
    // }

    // /* Options */
    // _print_options(state->fd, state->carg);

    // /* Footer */
    // _print_multiline(state->fd, state->carg->footer, 0, HELP_LINESIZE);
}
