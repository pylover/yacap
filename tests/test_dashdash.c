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
#include <cutest.h>

#include "carg.h"
#include "helpers.h"


struct barbaz {
    const char *args[8];
    int count;
};


static enum carg_eatstatus
_eater(const struct carg_option *opt, const char *value,
        struct barbaz *a) {
    if (opt) {
        return CARG_EAT_UNRECOGNIZED;
    }

    a->args[a->count++] = value;
    return CARG_EAT_OK;
}


static void
test_dashdash() {
    struct barbaz args;
    struct carg_option options[] = {
        {"bar", 'b', "BAR", 0, NULL},
        {"baz", 'z', "BAZ", 0, NULL},
        {NULL}
    };
    struct carg carg = {
        .eat = (carg_eater_t)_eater,
        .options = options,
        .args = "...",
        .header = NULL,
        .footer = NULL,
        .version = NULL,
        .userptr = &args,
        .flags = 0,
    };

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar baz", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo -- --bar -zbaz -- quux",
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
    eqint(CARG_OK, carg_parse_string(&carg, "qux -- foo bar baz", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo -- bar baz", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar -- baz", NULL));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.args[0]);
    eqstr("bar", args.args[1]);
    eqstr("baz", args.args[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar baz --", NULL));
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
