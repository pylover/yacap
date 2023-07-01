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


#include "tokenizer.c"


void
test_tokenizer() {
    const int count = 9;
    struct carg_token tok;
    struct carg_option options[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };
    char *argv[9] = {
        "foo",
        "-fbzoo",
        "bar",
        "-qux",
        "",
        "--foo=bar",
        "--foo=",
        "--",
        "--foo"
    };

    /* foo */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenize(options, count, argv, &tok));
    eqint(-1, tok.occurance);
    eqstr("foo", tok.value);
    isnull(tok.option);

    /* f */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenize(options, count, argv, &tok));
    eqint(1, tok.occurance);
    isnull(tok.value);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);

    /* b */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenize(options, count, argv, &tok));
    eqint(1, tok.occurance);
    isnotnull(tok.option);
    isnotnull(tok.value);
    eqchr('b', tok.option->key);
    eqstr("zoo", tok.value);

    /* bar */
    eqint(1, tokenize(options, count, argv, &tok));
    eqint(-1, tok.occurance);
    eqstr("bar", tok.value);
    isnull(tok.option);

    /* -qux */
    eqint(1, tokenize(options, count, argv, &tok));
    eqint(-1, tok.occurance);
    eqstr("-qux", tok.value);
    isnull(tok.option);

    /* --foo (option) */
    eqint(1, tokenize(options, count, argv, &tok));
    eqint(2, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqstr("bar", tok.value);

    /* --foo (option) */
    eqint(1, tokenize(options, count, argv, &tok));
    eqint(3, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqstr("", tok.value);

    /* --foo (positional) */
    eqint(1, tokenize(options, count, argv, &tok));
    eqint(-1, tok.occurance);
    eqstr("--foo", tok.value);
    isnull(tok.option);

    /* Termination */
    eqint(0, tokenize(options, count, argv, &tok));
    eqint(-1, tok.occurance);
    isnull(tok.value);
    isnull(tok.option);
}


int
main() {
    test_tokenizer();
    return EXIT_SUCCESS;
}
