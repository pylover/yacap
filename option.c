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
#include "internal.h"
#include "carg.h"
#include "option.h"


int
option_print(int fd, const struct carg_option *opt) {
    int bytes = 0;
    int status;

    if ((opt->key != 0) && ISCHAR(opt->key)) {
        status = dprintf(fd, "-%c%s", opt->key, opt->name? "/": "");
        if (status == -1) {
            return -1;
        }

        bytes += status;
    }

    if (opt->name) {
        status = dprintf(fd, "--%s", opt->name);
        if (status == -1) {
            return -1;
        }

        bytes += status;
    }

    return bytes;
}
