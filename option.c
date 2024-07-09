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
#include <ctype.h>

#include "config.h"
#include "carg.h"
#include "option.h"


static char _temp[CARG_TEMPBUFFSIZE];


const char *
option_repr(const struct carg_option *opt) {
    int c = 0;

    if (isalpha(opt->key)) {
        c += snprintf(_temp, CARG_TEMPBUFFSIZE, "-%c", opt->key);
    }

    if (c && opt->name) {
        c += snprintf(_temp + c, CARG_TEMPBUFFSIZE - c, "/");
    }

    if (opt->name) {
        c += snprintf(_temp + c, CARG_TEMPBUFFSIZE - c, "--%s", opt->name);
    }

    return _temp;
}
