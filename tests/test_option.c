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
    if (opt == NULL) {
        /* Positional */
        ERROR("Positional detected: %s", value);
        return CARG_EAT_UNRECOGNIZED;
    }
    switch (opt->key) {
        case 'f':
            args.foo = atoi(value);
            break;
        case 'b':
            args.bar = atoi(value);
            break;
        case 'z':
            args.baz += 1;
            break;
        case 'q':
            args.qux += 1;
            break;
        default:
            return CARG_EAT_UNRECOGNIZED;
    }

    return CARG_EAT_OK;
}


static void
test_duplicate_options() {
    struct carg_option options[] = {
        {"foo", 'f', "FOO", 0, "Foo option"},
        {"foo", 'f', "FOO", 0, "Foo option"},
        {NULL}
    };
    struct carg c = {
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

    eqint(CARG_ERROR, carg_parse_string(&c, "foo -f", NULL));
    eqstr("", out);
    eqstr("[carg] option duplicated -- '-f/--foo'\n", err);
}


static void
test_program_error() {
    struct carg_option options[] = {
        {"foo", 'f', "FOO", 0, "Foo option"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };
    struct carg c = {
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

    eqint(CARG_ERROR, carg_parse_string(&c, "foo -f", NULL));
    eqstr("", out);
    eqstr("foo: option requires an argument -- '-f/--foo'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(CARG_ERROR, carg_parse_string(&c, "foo --foo", NULL));
    eqstr("", out);
    eqstr("foo: option requires an argument -- '-f/--foo'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(CARG_ERROR, carg_parse_string(&c, "foo -F", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '-F'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(CARG_ERROR, carg_parse_string(&c, "foo --qux", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--qux'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(CARG_ERROR, carg_parse_string(&c, "foo --qux=", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--qux='\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(CARG_ERROR, carg_parse_string(&c, "foo -qthud", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '-q'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(CARG_ERROR, carg_parse_string(&c, "foo --qux=thud", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--qux=thud'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);
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

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "foo -f3", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(3, args.foo);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "foo --foo 4", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(4, args.foo);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "foo --foo=5", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(5, args.foo);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "foo -qzf2", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(2, args.foo);
    eqint(1, args.qux);
    eqint(1, args.baz);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "foo -qf 9", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(9, args.foo);
    eqint(1, args.qux);
    eqint(0, args.baz);
}


int
main() {
    test_duplicate_options();
    test_program_error();
    test_option_value();
    return EXIT_SUCCESS;
}
