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
#include <unistd.h>

#include <cutest.h>

#include "include/yacap.h"
#include "helpers.h"


static struct {
    int foo;
    int bar;
    int baz;
    int qux;
} args = {0, 0, 0, 0};


static enum yacap_eatstatus
eatarg(struct yacap_option *opt, const char *value) {
    if (opt == NULL) {
        /* Positional */
        dprintf(STDERR_FILENO, "Positional detected: %s", value);
        return YACAP_EAT_UNRECOGNIZED;
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
        case 'x':
            args.qux += 1;
            break;
        default:
            return YACAP_EAT_UNRECOGNIZED;
    }

    return YACAP_EAT_OK;
}


static void
test_options_duplicated() {
    struct yacap_option options[] = {
        {"foo", 'f', "FOO", 0, "Foo option"},
        {"foo", 'f', "FOO", 0, "Foo option"},
        {NULL}
    };
    struct yacap c = {
        .eat = NULL,
        .options = options,
        .args = NULL,
        .header = NULL,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
        .commands = NULL,
    };

    eqint(YACAP_FATAL, yacap_parse_string(&c, "foo -f", NULL));
    eqstr("", out);
    eqstr("option duplicated -- '-f/--foo'\n", err);
}


static void
test_user_error() {
    struct yacap_option options[] = {
        {"foo", 'f', "FOO", 0, "Foo option"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {"baz", 'z', NULL, 0, NULL},
        {NULL}
    };
    struct yacap c = {
        .eat = NULL,
        .options = options,
        .args = NULL,
        .header = NULL,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
        .commands = NULL,
    };

    eqint(YACAP_FATAL, yacap_parse_string(&c, "foo -z", NULL));
    eqstr("", out);
    eqstr("foo: option not eaten -- '-z/--baz'\n", err);

    eqint(YACAP_FATAL, yacap_parse_string(&c, "foo --baz", NULL));
    eqstr("", out);
    eqstr("foo: option not eaten -- '-z/--baz'\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo -f", NULL));
    eqstr("", out);
    eqstr("foo: option requires an argument -- '-f/--foo'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo --foo", NULL));
    eqstr("", out);
    eqstr("foo: option requires an argument -- '-f/--foo'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo -F", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '-F'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo --qux", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--qux'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo --qux=", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--qux='\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo -xthud", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '-x'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo --qux=thud", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--qux=thud'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo --x", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--x'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(YACAP_USERERROR, yacap_parse_string(&c, "foo --f", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--f'\n"
          "Try `foo --help' or `foo --usage' for more information.\n", err);
}


static void
test_option_value() {
    struct yacap_option options[] = {
        {"foo", 'f', "FOO", 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {"baz", 'z', NULL, 0, NULL},
        {"qux", 'x', NULL, 0, NULL},
        {NULL}
    };
    struct yacap yacap = {
        .args = NULL,
        .header = NULL,
        .eat = (yacap_eater_t)eatarg,
        .options = options,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
    };

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo -f3", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(3, args.foo);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo --foo 4", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(4, args.foo);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo --foo=5", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(5, args.foo);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo -xzf2", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(2, args.foo);
    eqint(1, args.qux);
    eqint(1, args.baz);

    memset(&args, 0, sizeof(args));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo -xf 9", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(9, args.foo);
    eqint(1, args.qux);
    eqint(0, args.baz);
}


int
main() {
    test_user_error();
    test_options_duplicated();
    test_option_value();
    return EXIT_SUCCESS;
}
