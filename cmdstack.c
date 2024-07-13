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

#include "config.h"
#include "cmdstack.h"


void
cmdstack_init(struct cmdstack *s) {
    s->len = 0;
}


int
cmdstack_push(struct cmdstack *s, const char *name) {
    if (s->len >= CARG_CMDSTACK_MAX) {
        return -1;
    }

    s->names[s->len++] = name;

    return (int)s->len;
}


int
cmdstack_print(int fd, struct cmdstack *s) {
    int i;
    int bytes = 0;
    int status;

    if (!s->len) {
        return -1;
    }

    for (i = 0; i < s->len; i++) {
        status = dprintf(fd, "%s%s", i? " ": "", s->names[i]);
        if (status == -1) {
            return -1;
        }
        bytes += status;
    }

    return bytes;
}
