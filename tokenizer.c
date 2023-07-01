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


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <clog.h>

#include "option.h"
#include "tokenizer.h"


/* Coroutine  stuff*/
#define CSTART \
    static int __cline__ = 0; \
    switch (__cline__) { \
        case 0:


#define CREJECT goto cfinally


#define YIELD_OPT(o, v) do { \
        __cline__ = __LINE__; \
        token->value = v; \
        token->option = o; \
        token->occurance = ++state[(o)->key]; \
        return 1; \
        case __LINE__:; \
    } while (0)


#define YIELD_ARG(v) do { \
        __cline__ = __LINE__; \
        token->value = v; \
        token->option = NULL; \
        token->occurance = -1; \
        return 1; \
        case __LINE__:; \
    } while (0)


#define CEND } cfinally: \
    __cline__ = 0; \
    token->value = NULL; \
    token->option = NULL; \
    token->occurance = -1; \
    return 0


int
tokenize(struct carg_option *options, int argc, char **argv,
        struct carg_token *token) {
    // TODO: allocate options state nargs [len(options)]

    static int i;
    static int j;
    static int toklen;
    static const char *tok;
    static int state[sizeof(char)];
    static struct carg_option *opt = NULL;
    static struct carg_option *opt2 = NULL;
    const char *eq;
    static bool dashdash = false;

    memset(state, 0, sizeof(int) * sizeof(char));
    CSTART;
    for (i = 0; i < argc; i++) {
        tok = argv[i];
        opt = NULL;
        opt2 = NULL;

        if (tok == NULL) {
            CREJECT;
        }

        toklen = strlen(tok);
        if (toklen == 0) {
            continue;
        }

        if ((toklen == 1) || dashdash) {
            goto positional;
        }

        if ((tok[0] == '-') && (tok[1] == '-')) {
            if (toklen == 2) {
                dashdash = true;
                continue;
            }

            /* Double dashes option: '-foo' or '--foo=bar' */
            eq = strchr(tok, '=');
            opt = option_findbyname(options, tok + 2,
                    (eq? eq - tok: toklen) - 2);

            if (opt == NULL) {
                goto positional;
            }

            YIELD_OPT(opt, eq? eq+1: NULL);
            continue;
        }

        if (tok[0] == '-') {
            /* Single dash option: -f */
            opt = option_findbykey(options, tok[1]);
            if (opt == NULL) {
                goto positional;
            }

            for (j = 2; j < toklen; j++) {
                opt2 = option_findbykey(options, tok[j]);
                if (opt2 == NULL) {
                    YIELD_OPT(opt, tok + j);
                    break;
                }

                YIELD_OPT(opt, NULL);
                opt = opt2;
            }

            continue;
        }

        /* Positional argument */
positional:
        YIELD_ARG(tok);
    }

    CEND;
}
