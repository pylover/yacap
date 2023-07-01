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
#define START switch (t->line) { case 0:
#define REJECT goto finally


#define YIELD_OPT(o, v) do { \
        t->line = __LINE__; \
        token->value = v; \
        token->option = o; \
        token->occurance = ++(t->occurances[(o)->key]); \
        return 0; \
        case __LINE__:; \
    } while (0)


#define YIELD_ARG(v) do { \
        t->line = __LINE__; \
        token->value = v; \
        token->option = NULL; \
        token->occurance = -1; \
        return 0; \
        case __LINE__:; \
    } while (0)


#define END } finally: \
    t->line = 0; \
    token->value = NULL; \
    token->option = NULL; \
    token->occurance = -1; \
    return -1


struct tokenizer {
    struct carg_option *options;
    int argc;
    char **argv;

    /* tokenizer state */
    int line;
    int w;
    int c;
    int toklen;
    const char *tok;
    int occurances[256];
    struct carg_option *opt;
    struct carg_option *opt2;
    bool dashdash;
};


struct tokenizer *
tokenizer_new(struct carg_option *options, int argc, char **argv) {
    struct tokenizer *t = malloc(sizeof(struct tokenizer));
    if (t == NULL) {
        return NULL;
    }

    t->line = 0;
    t->options = options;
    t->argc = argc;
    t->argv = argv;
    t->dashdash = false;
    memset(t->occurances, 0, 255 * sizeof(int));
    return t;
}

void
tokenizer_dispose(struct tokenizer *t) {
    if (t == NULL) {
        return;
    }

    free(t);
}


int
tokenizer_next(struct tokenizer *t, struct carg_token *token) {
    const char *eq;

    START;
    for (t->w = 0; t->w < t->argc; t->w++) {
        t->tok = t->argv[t->w];
        t->opt = NULL;
        t->opt2 = NULL;

        if (t->tok == NULL) {
            REJECT;
        }

        t->toklen = strlen(t->tok);
        if (t->toklen == 0) {
            continue;
        }

        if ((t->toklen == 1) || t->dashdash) {
            goto positional;
        }

        if ((t->tok[0] == '-') && (t->tok[1] == '-')) {
            if (t->toklen == 2) {
                t->dashdash = true;
                continue;
            }

            /* Double dashes option: '-foo' or '--foo=bar' */
            eq = strchr(t->tok, '=');
            t->opt = option_findbyname(t->options, t->tok + 2,
                    (eq? eq - t->tok: t->toklen) - 2);

            if (t->opt == NULL) {
                goto positional;
            }

            YIELD_OPT(t->opt, eq? eq+1: NULL);
            continue;
        }

        if (t->tok[0] == '-') {
            /* Single dash option: -f */
            t->opt = option_findbykey(t->options, t->tok[1]);
            if (t->opt == NULL) {
                goto positional;
            }

            for (t->c = 2; t->c < t->toklen; t->c++) {
                t->opt2 = option_findbykey(t->options, t->tok[t->c]);
                if (t->opt2 == NULL) {
                    YIELD_OPT(t->opt, t->tok + t->c);
                    break;
                }

                YIELD_OPT(t->opt, NULL);
                t->opt = t->opt2;
            }

            continue;
        }

        /* Positional argument */
positional:
        YIELD_ARG(t->tok);
    }

    END;
}
