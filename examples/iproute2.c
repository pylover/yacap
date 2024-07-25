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

#include "carg.h"


typedef int (*cmdmain_t) ();
static struct carg_subcommand add = {
    .name = "add",
};


static struct carg_subcommand route = {
    .name = "route",
    .commands = (const struct carg_subcommand*[]) {
        &add,
        NULL
    },
};


/* create and configure a CArg structure */
static struct carg cli = {
    .options = NULL,
    .commands = (const struct carg_subcommand*[]) {
        &route,
        NULL
    },
    .args = NULL,
    .header = NULL,
    .footer = NULL,
    .eat = NULL,
    .userptr = NULL,
    .version = NULL,
    .flags = 0,
};


int
main(int argc, const char **argv) {
    const struct carg_subcommand *cmd;
    enum carg_status status = carg_parse(&cli, argc, argv, &cmd);

    if (status == CARG_OK_EXIT) {
        return EXIT_SUCCESS;
    }
    else if (status < CARG_OK) {
        return EXIT_FAILURE;
    }

    if (cmd) {
        return ((cmdmain_t)cmd->userptr)();
    }

    carg_usage_print(&cli);
    carg_dispose(&cli);
    return 0;
}
