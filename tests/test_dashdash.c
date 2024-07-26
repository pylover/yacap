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

#include "yacap.h"
#include "helpers.h"


struct barbaz {
    const char *args[8];
    int count;
};


static enum yacap_eatstatus
_eater(const struct yacap_option *opt, const char *value,
        struct barbaz *a) {
    if (opt) {
        return YACAP_EAT_UNRECOGNIZED;
    }

    a->args[a->count++] = value;
    return YACAP_EAT_OK;
}


static void
test_dashdash() {
    struct barbaz args;
    struct yacap_option options[] = {
        {"bar", 'b', "BAR", 0, NULL},
        {"baz", 'z', "BAZ", 0, NULL},
        {NULL}
    };
    struct yacap yacap = {
        .eat = (yacap_eater_t)_eater,
        .options = options,
        .args = "...",
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
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "qux foo -- --bar -zbaz -- quux",
                NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("--bar", args.args[1]);
    eqstr("-zbaz", args.args[2]);
    eqstr("--", args.args[3]);
    eqstr("quux", args.args[4]);
    eqint(5, args.count);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "qux -- foo bar baz", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "qux foo -- bar baz", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "qux foo bar -- baz", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "qux foo bar baz --", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);
}


int
main() {
    test_dashdash();
    return EXIT_SUCCESS;
}
