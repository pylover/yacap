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
#include <stdlib.h>

#include <clog.h>
#include <cutest.h>


#include "carg.c"


// TODO: quotes "", ''


void
test_tokenizer() {
#define TOTAL   14
    struct carg_token tok;
    struct carg_optiondb optdb;
    struct carg_option options1[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };
    struct carg_option options2[] = {
        {"baz", 'z', "[BAZ]", 0, "baz option"},
        {NULL}
    };


    const char *argv[TOTAL] = {
        "foo",
        "-fthud",
        "-fbbar",
        "bar",
        "-qux",
        "",
        "--foo=bar",
        "--foo=bar baz",
        "--foo=",
        "--foo",
        "--baz=baz",
        "-zzoo",
        "--",
        "--foo",
    };

    optiondb_init(&optdb);
    optiondb_insert(&optdb, options1);
    optiondb_insert(&optdb, options2);
    struct tokenizer *t = tokenizer_new(TOTAL, argv, &optdb);
    isnotnull(t);

    /* foo */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("foo", tok.text);
    isnull(tok.option);

    /* f */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(1, tok.occurance);
    isnull(tok.text);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);

    /* thud */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("thud", tok.text);
    isnull(tok.option);

    /* f */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(2, tok.occurance);
    isnull(tok.text);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);

    /* b */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(1, tok.occurance);
    isnotnull(tok.option);
    isnotnull(tok.text);
    eqchr('b', tok.option->key);
    eqstr("bar", tok.text);

    /* bar */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("bar", tok.text);
    isnull(tok.option);

    /* -qux */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("-qux", tok.text);
    isnull(tok.option);

    /* --foo=bar (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(3, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqstr("bar", tok.text);

    /* --foo=bar baz (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(4, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqstr("bar baz", tok.text);

    /* --foo= (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(5, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqstr("", tok.text);

    /* --foo (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(6, tok.occurance);
    isnotnull(tok.option);
    isnull(tok.text);
    eqchr('f', tok.option->key);

    /* --baz (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(1, tok.occurance);
    isnotnull(tok.option);
    eqchr('z', tok.option->key);
    eqstr("baz", tok.text);

    /* --baz (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(2, tok.occurance);
    isnotnull(tok.option);
    eqchr('z', tok.option->key);
    eqstr("zoo", tok.text);

    /* --foo (positional) */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("--foo", tok.text);
    isnull(tok.option);

    /* Termination */
    memset(&tok, 0, sizeof(tok));
    eqint(0, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    isnull(tok.text);
    isnull(tok.option);

    tokenizer_dispose(t);
}


void
test_tokenizer_error() {
    struct carg_token tok;
    struct carg_optiondb optdb;
    struct carg_option options1[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };
    struct carg_option options2[] = {
        {"baz", 'z', "[BAZ]", 0, "baz option"},
        {NULL}
    };

    const char *argv[3] = {
        "foo",
        NULL,
        "foo",
    };

    optiondb_init(&optdb);
    optiondb_insert(&optdb, options1);
    optiondb_insert(&optdb, options2);
    struct tokenizer *t = tokenizer_new(3, argv, &optdb);
    isnotnull(t);

    /* foo */
    memset(&tok, 0, sizeof(tok));
    eqint(1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("foo", tok.text);
    isnull(tok.option);

    /* NULL */
    memset(&tok, 0, sizeof(tok));
    eqint(-1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    isnull(tok.text);
    isnull(tok.option);

    /* NULL (Again) */
    memset(&tok, 0, sizeof(tok));
    eqint(-1, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    isnull(tok.text);
    isnull(tok.option);

    tokenizer_dispose(t);
}


int
main() {
    test_tokenizer();
    test_tokenizer_error();
    return EXIT_SUCCESS;
}
