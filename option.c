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


#include <stdlib.h>
#include <string.h>

#include "option.h"


#define CMP(x, y, l) (strncmp(x, y, l) == 0)


int
option_count(struct carg_option *options) {
    if (options == NULL) {
        return -1;
    }

    struct carg_option *opt = options;
    int count = 0;

    while (opt->name) {
        count++;
        opt++;
    }

    return count;
}


struct carg_option *
option_findbykey(struct carg_option *options[], int count, int key) {
    int i;
    struct carg_option *opt;

    for (i = 0; i < count; i++) {
        opt = options[i];

        while (opt->name) {
            if (opt->key == key) {
                return opt;
            }

            opt++;
        }
    }

    return NULL;
}


struct carg_option *
option_findbyname(struct carg_option *options[], int count,
        const char *name, int len) {
    int i;
    struct carg_option *opt;

    for (i = 0; i < count; i++) {
        opt = options[i];

        while (opt->name) {
            if (CMP(name, opt->name, len)) {
                return opt;
            }

            opt++;
        }
    }

    return NULL;
}
