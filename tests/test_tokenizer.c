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


#include "carg.c"


void
test_tokenizer() {
    int count = 5;
    char *argv[5] = {"foo", "-zbfoo", "bar", "", "--foo"};
    int len;
    const char *tok;
    struct carg_option *opt;

    struct carg_option options[] = {
        {"foo", 'z', NULL, 0, "Foo flag"},
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
        .flags = 0,
    };

    /* foo */
    eqint(3, _tokenize(&carg, count, argv, &tok, &opt));
    eqnstr("foo", tok, 3);
    isnull(opt);

    /* z */
    eqint(1, _tokenize(&carg, count, argv, &tok, &opt));
    eqchr('z', tok[0]);
    isnotnull(opt);

    /* b */
    eqint(1, _tokenize(&carg, count, argv, &tok, &opt));
    eqchr('b', tok[0]);
    isnotnull(opt);

    eqint(3, _tokenize(&carg, count, argv, &tok, &opt));
    eqnstr("foo", tok, 3);
    isnull(opt);

    eqint(3, _tokenize(&carg, count, argv, &tok, &opt));
    eqnstr("bar", tok, 3);
    isnull(opt);

    eqint(5, _tokenize(&carg, count, argv, &tok, &opt));
    eqnstr("--foo", tok, 5);
    isnull(opt);
}


int
main() {
    test_tokenizer();
    return EXIT_SUCCESS;
}
