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


#include "tokenizer.h"


void
test_tokenizer() {
#define C   10
    struct carg_token tok;
    struct carg_option options[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };
    char *argv[C] = {
        "foo",
        "-fbzoo",
        "bar",
        "-qux",
        "",
        "--foo=bar",
        "--foo=",
        "--foo",
        "--",
        "--foo"
    };

    struct tokenizer *t = tokenizer_new(options, C, argv);
    isnotnull(t);

    /* foo */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("foo", tok.value);
    isnull(tok.option);

    /* f */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(1, tok.occurance);
    isnull(tok.value);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);

    /* b */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(1, tok.occurance);
    isnotnull(tok.option);
    isnotnull(tok.value);
    eqchr('b', tok.option->key);
    eqstr("zoo", tok.value);

    /* bar */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("bar", tok.value);
    isnull(tok.option);

    /* -qux */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("-qux", tok.value);
    isnull(tok.option);

    /* --foo=bar (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(2, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqstr("bar", tok.value);

    /* --foo= (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(3, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqstr("", tok.value);

    /* --foo (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(4, tok.occurance);
    isnotnull(tok.option);
    isnull(tok.value);
    eqchr('f', tok.option->key);

    /* --foo (positional) */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("--foo", tok.value);
    isnull(tok.option);

    /* Termination */
    memset(&tok, 0, sizeof(tok));
    eqint(-1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    isnull(tok.value);
    isnull(tok.option);

    tokenizer_dispose(t);
}


int
main() {
    test_tokenizer();
    return EXIT_SUCCESS;
}
