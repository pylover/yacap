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


#define ARGVSIZE(a) (sizeof(a) / sizeof(char*))
// TODO: quotes "", ''


void
test_tokenizer() {
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


    const char *argv[] = {
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
        "--baz=_baz_",
        "-zzoo",
        "--",
        "--foo",
    };

    optiondb_init(&optdb);
    optiondb_insertvector(&optdb, options1);
    optiondb_insertvector(&optdb, options2);
    struct tokenizer *t = tokenizer_new(ARGVSIZE(argv), argv, &optdb);
    isnotnull(t);

    /* foo */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_POSITIONAL, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("foo", tok.text);
    eqint(3, tok.len);
    isnull(tok.option);

    /* f */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(1, tok.occurance);
    isnull(tok.text);
    eqint(0, tok.len);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);

    /* thud */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_UNKNOWN, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("thud", tok.text);
    eqint(1, tok.len);
    isnull(tok.option);

    /* f */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(2, tok.occurance);
    isnull(tok.text);
    eqint(0, tok.len);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);

    /* b */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(1, tok.occurance);
    isnotnull(tok.option);
    isnotnull(tok.text);
    eqchr('b', tok.option->key);
    eqstr("bar", tok.text);
    eqint(3, tok.len);

    /* bar */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_POSITIONAL, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    isnull(tok.option);
    eqstr("bar", tok.text);
    eqint(3, tok.len);

    /* -qux */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_UNKNOWN, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqnstr("q", tok.text, tok.len);
    eqint(1, tok.len);
    isnull(tok.option);

    /* --foo=bar (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(3, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqnstr("bar", tok.text, tok.len);

    /* --foo=bar baz (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(4, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqnstr("bar baz", tok.text, tok.len);
    eqint(7, tok.len);

    /* --foo= (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(5, tok.occurance);
    isnotnull(tok.option);
    eqchr('f', tok.option->key);
    eqstr("", tok.text);
    eqint(0, tok.len);

    /* --foo (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(6, tok.occurance);
    isnotnull(tok.option);
    isnull(tok.text);
    eqint(0, tok.len);
    eqchr('f', tok.option->key);

    /* --baz=baz (option) */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(1, tok.occurance);
    isnotnull(tok.option);
    eqchr('z', tok.option->key);
    eqnstr("_baz_", tok.text, tok.len);
    eqint(5, tok.len);

    /* -zzoo (baz option) */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_OPTION, tokenizer_next(t, &tok));
    eqint(2, tok.occurance);
    isnotnull(tok.option);
    eqchr('z', tok.option->key);
    eqstr("zoo", tok.text);
    eqint(3, tok.len);

    /* --foo (positional) */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_POSITIONAL, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqnstr("--foo", tok.text, tok.len);
    eqint(5, tok.len);
    isnull(tok.option);

    /* Termination */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_END, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    isnull(tok.text);
    eqint(0, tok.len);
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
    optiondb_insertvector(&optdb, options1);
    optiondb_insertvector(&optdb, options2);
    struct tokenizer *t = tokenizer_new(3, argv, &optdb);
    isnotnull(t);

    /* foo */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_POSITIONAL, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    eqstr("foo", tok.text);
    isnull(tok.option);

    /* NULL */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_ERROR, tokenizer_next(t, &tok));
    eqint(-1, tok.occurance);
    isnull(tok.text);
    isnull(tok.option);

    /* NULL (Again) */
    memset(&tok, 0, sizeof(tok));
    eqint(CARG_TOK_ERROR, tokenizer_next(t, &tok));
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
