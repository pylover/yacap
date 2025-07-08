// Copyright 2023 Vahid Mardani
/*
 * This file is part of yacap.
 *  yacap is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  yacap is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with yacap. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "helpers.h"


struct yacap_command *
command_findbyname(const struct yacap_command *cmd, const char *name) {
    if (name == NULL) {
        return NULL;
    }

    if ((cmd == NULL) || (!cmd->commands) || (!cmd->commands[0])) {
        return NULL;
    }

    struct yacap_command * const *c = cmd->commands;
    struct yacap_command *s;

    while ((s = *c)) {
        if (STREQ(name, s->name)) {
            return s;
        }

        c++;
    }

    return NULL;
}
