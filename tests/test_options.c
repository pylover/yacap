// Copyright 2023 Vahid Mardani
/*
 * This file is part of Carrow.
 *  Carrow is free software: you can redistribute it and/or modify it under 
 *  the terms of the GNU General Public License as published by the Free 
 *  Software Foundation, either version 3 of the License, or (at your option) 
 *  any later version.
 *  
 *  Carrow is distributed in the hope that it will be useful, but WITHOUT ANY 
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 *  details.
 *  
 *  You should have received a copy of the GNU General Public License along 
 *  with Carrow. If not, see <https://www.gnu.org/licenses/>. 
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
} args = {0, 0, 0};


static enum carg_eatresult
eatarg(int key, const char *value, struct carg_state *state) {
    switch (key) {
        case 'f':
            args.foo = atoi(value);
            break;
        case 'b':
            args.bar = atoi(value);
            break;
        case 'z':
            args.baz = 1;
            break;
        default:
            return CARG_NOT_EATEN;
    }

    return CARG_EATEN;
}


static void
test_program_error() {
    struct carg_option options[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };
    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .eat = NULL,
        .options = options,
        .footer = NULL,
        .version = NULL,
    };

    char out[1024] = "\0";
    char err[1024] = "\0";

    eqint(CARG_ERR,
            carg_parse_string(&carg, out, err, "foo -f", NULL));
    eqstr("", out);
    eqstr("foo: -f: (PROGRAM ERROR) Option should have been recognized!?\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);
}


static void
test_option_value() {
    struct carg_option options[] = {
        {"foo", 'f', "FOO", 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {"baz", 'z', NULL, 0, NULL},
        {NULL}
    };
    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .eat = eatarg,
        .options = options,
        .footer = NULL,
        .version = NULL,
    };

    char out[1024] = "\0";
    char err[1024] = "\0";

    eqint(CARG_ERR,
            carg_parse_string(&carg, out, err, "foo -f", NULL));
    eqstr("", out);
    eqstr("foo: option requires an argument -- '-f'\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(CARG_ERR,
            carg_parse_string(&carg, out, err, "foo --foo5", NULL));
    eqstr("", out);
    eqstr("foo: unrecognized option '--foo5'\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK,
            carg_parse_string(&carg, out, err, "foo -f3", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(3, args.foo);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK,
            carg_parse_string(&carg, out, err, "foo --foo 4", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(4, args.foo);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK,
            carg_parse_string(&carg, out, err, "foo --foo=5", NULL));
    eqstr("", out);
    eqstr("", err);
    eqint(5, args.foo);

    eqint(CARG_ERR,
            carg_parse_string(&carg, out, err, "foo -z2", NULL));
    eqstr("", out);
    eqstr("foo: unrecognized option '-z2'\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);
}


int
main() {
    test_program_error();
    test_option_value();
    return EXIT_SUCCESS;
}
