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

#include "carg.h"


typedef int (*cmdmain_t) ();


static int
_main(const struct carg *c, const struct carg_command *cmd) {
    carg_commandchain_print(STDERR_FILENO, c);
    dprintf(STDERR_FILENO, ": Invalid command\n");
    carg_try_help(c);
    return 0;
}


static int
_route_add(const struct carg *c, const struct carg_command *cmd) {
    printf("Adding route: TODO\n");
    return 0;
}


static struct carg_command add = {
    .name = "add",
    .entrypoint = _route_add,
};


static struct carg_command route = {
    .name = "route",
    .commands = (const struct carg_command*[]) {
        &add,
        NULL
    },
};


/* create and configure a CArg structure */
static struct carg cli = {
    .commands = (const struct carg_command*[]) {
        &route,
        NULL
    },
    .entrypoint = _main,
};


int
main(int argc, const char **argv) {
    int ret = EXIT_FAILURE;
    const struct carg_command *cmd;
    enum carg_status status = carg_parse(&cli, argc, argv, &cmd);

    if (status == CARG_OK_EXIT) {
        ret = EXIT_SUCCESS;
    }
    else if ((status == CARG_OK) && cmd) {
        ret = cmd->entrypoint(&cli, cmd);
    }

    carg_dispose(&cli);
    return ret;
}
