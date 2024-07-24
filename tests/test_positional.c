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


struct foobarbaz {
    const char* foo;
    const char* bar;
    const char* baz;
};


static enum carg_eatstatus
_eater(const struct carg_option *opt, const char *value,
        struct foobarbaz *a) {
    if (opt) {
        return CARG_EAT_UNRECOGNIZED;
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
        return CARG_EAT_UNRECOGNIZED;
    }
    return CARG_EAT_OK;
}


static void
test_positionals() {
    struct foobarbaz args = {NULL, NULL, NULL};
    struct carg carg = {
        .eat = (carg_eater_t)_eater,
        .options = NULL,
        .args = "FOO BAR BAZ",
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
    eqstr("foo", args.foo);
    eqstr("bar", args.bar);
    eqstr("baz", args.baz);

    memset(&args, 0, sizeof(args));
    eqint(CARG_USERERROR,
            carg_parse_string(&carg, "qux foo bar baz thud", NULL));
    eqstr("", out);
    eqstr("qux: invalid argument -- 'thud'\n"
        "Try `qux --help' or `qux --usage' for more information.\n", err);

    memset(&args, 0, sizeof(args));
    eqint(CARG_USERERROR, carg_parse_string(&carg, "qux foo bar", NULL));
    eqstr("", out);
    eqstr("qux: invalid positional arguments count\n"
        "Try `qux --help' or `qux --usage' for more information.\n", err);
}


// static void
// test_dashdash() {
//     struct fooargs args;
//     struct carg_option options[] = {
//         {"bar", 'b', "BAR", 0, NULL},
//         {"baz", 'z', "BAZ", 0, NULL},
//         {NULL}
//     };
//     struct carg carg = {
//         .eat = eat_fooargs,
//         .options = options,
//         .args = NULL,
//         .header = NULL,
//         .footer = NULL,
//         .version = NULL,
//         .flags = 0,
//     };
//
//     memset(&args, 0, sizeof(args));
//     eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar baz", &args));
//     eqstr("", out);
//     eqstr("", err);
//     eqstr("foo", args.foos[0]);
//     eqstr("bar", args.foos[1]);
//     eqstr("baz", args.foos[2]);
//     eqint(3, args.count);
//
//     memset(&args, 0, sizeof(args));
//     eqint(CARG_OK, carg_parse_string(&carg, "qux foo -- --bar -zbaz -- quux",
//                 &args));
//     eqstr("", out);
//     eqstr("", err);
//     eqstr("foo", args.foos[0]);
//     eqstr("--bar", args.foos[1]);
//     eqstr("-zbaz", args.foos[2]);
//     eqstr("--", args.foos[3]);
//     eqstr("quux", args.foos[4]);
//     eqint(5, args.count);
//
//     memset(&args, 0, sizeof(args));
//     eqint(CARG_OK, carg_parse_string(&carg, "qux -- foo bar baz", &args));
//     eqstr("", out);
//     eqstr("", err);
//     eqstr("foo", args.foos[0]);
//     eqstr("bar", args.foos[1]);
//     eqstr("baz", args.foos[2]);
//     eqint(3, args.count);
//
//     memset(&args, 0, sizeof(args));
//     eqint(CARG_OK, carg_parse_string(&carg, "qux foo -- bar baz", &args));
//     eqstr("", out);
//     eqstr("", err);
//     eqstr("foo", args.foos[0]);
//     eqstr("bar", args.foos[1]);
//     eqstr("baz", args.foos[2]);
//     eqint(3, args.count);
//
//     memset(&args, 0, sizeof(args));
//     eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar -- baz", &args));
//     eqstr("", out);
//     eqstr("", err);
//     eqstr("foo", args.foos[0]);
//     eqstr("bar", args.foos[1]);
//     eqstr("baz", args.foos[2]);
//     eqint(3, args.count);
//
//     memset(&args, 0, sizeof(args));
//     eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar baz --", &args));
//     eqstr("", out);
//     eqstr("", err);
//     eqstr("foo", args.foos[0]);
//     eqstr("bar", args.foos[1]);
//     eqstr("baz", args.foos[2]);
//     eqint(3, args.count);
// }


int
main() {
    test_positionals();
    return EXIT_SUCCESS;
}
