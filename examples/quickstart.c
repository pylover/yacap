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
#include <stdio.h>
#include <stdlib.h>

#include "include/yacap.h"


static enum yacap_eatstatus
_eat(const struct yacap_option *opt, const char *value) {
    if (opt == NULL) {
        /* Positional */
        printf("Positional: %s\n", value);
        return YACAP_EAT_OK;
    }

    switch (opt->key) {
        case 'f':
            printf("foo option: --%s=%s\n", opt->name, value);
            break;
        case 'b':
            printf("bar option: --%s=%s\n", opt->name, value);
            break;
        case 'z':
            printf("baz flag: --%s=%s\n", opt->name, value);
            break;
        default:
            return YACAP_EAT_UNRECOGNIZED;
    }

    return YACAP_EAT_OK;
}


/* create and configure a yacap structure */
static struct yacap cli = {
    .eat = (yacap_eater_t)_eat,
    .options = (const struct yacap_option[]) {
        {"foo", 'f', "FOO", 0, "Foo option with value"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {"baz", 'z', NULL, 0, NULL},
        {NULL}  // vector termination
    },
};


int
main(int argc, const char **argv) {
    enum yacap_status status = yacap_parse(&cli, argc, argv, NULL);
    yacap_dispose(&cli);

    if (status >= YACAP_OK) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
