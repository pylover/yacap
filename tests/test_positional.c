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
#include <cutest.h>

#include "include/yacap.h"
#include "helpers.h"


struct foobarbaz {
    const char* foo;
    const char* bar;
    const char* baz;
};


static enum yacap_eatstatus
_eater(const struct yacap_option *opt, const char *value,
        struct foobarbaz *a) {
    if (opt) {
        return YACAP_EAT_UNRECOGNIZED;
    }

    if (a->foo == NULL) {
        a->foo = value;
    }
    else if (a->bar == NULL) {
        a->bar = value;
    }
    else if (a->baz == NULL) {
        a->baz = value;
    }
    else {
        return YACAP_EAT_UNRECOGNIZED;
    }
    return YACAP_EAT_OK;
}


static void
test_positionals() {
    struct foobarbaz args = {NULL, NULL, NULL};
    struct yacap yacap = {
        .eat = (yacap_eater_t)_eater,
        .options = NULL,
        .args = "FOO BAR BAZ",
        .header = NULL,
        .footer = NULL,
        .version = NULL,
        .userptr = &args,
        .flags = 0,
    };

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "qux foo bar baz", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foo);
    eqstr("bar", args.bar);
    eqstr("baz", args.baz);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_USERERROR,
            yacap_parse_string(&yacap, "qux foo bar baz thud", NULL));
    eqstr("", out);
    eqstr("qux: invalid argument -- 'thud'\n"
        "Try `qux --help' or `qux --usage' for more information.\n", err);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_USERERROR, yacap_parse_string(&yacap, "qux foo bar", NULL));
    eqstr("", out);
    eqstr("qux: invalid positional arguments count\n"
        "Try `qux --help' or `qux --usage' for more information.\n", err);
}


int
main() {
    test_positionals();
    return EXIT_SUCCESS;
}
