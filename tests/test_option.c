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


#include <clog.h>
#include <cutest.h>

#include "carg.h"
#include "helpers.h"


static struct {
    int foo;
    int bar;
    int baz;
    int qux;
} args = {0, 0, 0, 0};


static enum carg_eatstatus
eatarg(struct carg_option *opt, const char *value) {
    switch (opt->key) {
        case 'f':
            args.foo = atoi(value);
            break;
        case 'b':
            args.bar = atoi(value);
            break;
        case 'z':
            args.baz = 1;
            break;

        case '1':
            args.qux = 1;
            break;

        default:
            return CARG_EAT_UNRECOGNIZED;
    }

    return CARG_EAT_OK;
}


static void
test_program_error() {
    struct carg_option options[] = {
        {"foo", 'f', "FOO", 0, "Foo option"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };
    struct carg carg = {
        .eat = NULL,
        .options = options,
        .args = NULL,
        .header = NULL,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
        .commands = NULL,
    };

    clog_verbosity = CLOG_INFO;
    eqint(CARG_ERROR, carg_parse_string(&carg, "foo -F", NULL));
    eqstr("", out);
    eqstr("[error] foo: -F: (PARSE ERROR) Option should have been "
          "recognized!?\nTry `foo --help' or `foo --usage' for more "
          "information.\n", err);
}


static void
test_option_value() {
    struct carg_option options[] = {
        {"foo", 'f', "FOO", 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {"baz", 'z', NULL, 0, NULL},
        {"qux", 'q', NULL, 0, NULL},
        {NULL}
    };
    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .eat = (carg_eater)eatarg,
        .options = options,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
    };

    // eqint(CARG_ERR, carg_parse_string(&carg, "foo -f", NULL, NULL));
    // eqstr("", out);
    // eqstr("foo: '-f' option requires an argument\n"
    //     "Try `foo --help' or `foo --usage' for more information.\n", err);

    // eqint(CARG_ERR, carg_parse_string(&carg, "foo --foo5", NULL, NULL));
    // eqstr("", out);
    // eqstr("foo: unrecognized option '--foo5'\n"
    //     "Try `foo --help' or `foo --usage' for more information.\n", err);

    // memset(&args, 0, sizeof(args));
    // eqint(CARG_OK, carg_parse_string(&carg, "foo -f3", NULL, NULL));
    // eqstr("", out);
    // eqstr("", err);
    // eqint(3, args.foo);

    // memset(&args, 0, sizeof(args));
    // eqint(CARG_OK, carg_parse_string(&carg, "foo --foo 4", NULL, NULL));
    // eqstr("", out);
    // eqstr("", err);
    // eqint(4, args.foo);

    // memset(&args, 0, sizeof(args));
    // eqint(CARG_OK, carg_parse_string(&carg, "foo --foo=5", NULL, NULL));
    // eqstr("", out);
    // eqstr("", err);
    // eqint(5, args.foo);

    // eqint(CARG_ERR, carg_parse_string(&carg, "foo -z2", NULL, NULL));
    // eqstr("", out);
    // eqstr("foo: unrecognized option '-z2'\n"
    //     "Try `foo --help' or `foo --usage' for more information.\n", err);

    // memset(&args, 0, sizeof(args));
    // eqint(CARG_OK, carg_parse_string(&carg, "foo -qzf2", NULL, NULL));
    // eqstr("", out);
    // eqstr("", err);
    // eqint(2, args.foo);
    // eqint(1, args.qux);
    // eqint(1, args.baz);

    // memset(&args, 0, sizeof(args));
    // eqint(CARG_OK, carg_parse_string(&carg, "foo -qf 9", NULL, NULL));
    // eqstr("", out);
    // eqstr("", err);
    // eqint(9, args.foo);
    // eqint(1, args.qux);
    // eqint(0, args.baz);
}


int
main() {
    // test_program_error();
    // test_option_value();
    return EXIT_SUCCESS;
}
