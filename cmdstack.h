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
#ifndef CMDSTACK_H_
#define CMDSTACK_H_


#include "config.h"


struct cmdstack {
    const char *names[YACAP_CMDSTACK_MAX];
    const struct yacap_command *commands[YACAP_CMDSTACK_MAX];
    unsigned char len;
};


void
cmdstack_init(struct cmdstack *s);


int
cmdstack_push(struct cmdstack *s, const char *name,
        const struct yacap_command *cmd);


const struct yacap_command *
cmdstack_last(struct cmdstack *s);


int
cmdstack_print(int fd, struct cmdstack *s);


#endif  // CMDSTACK_H_
