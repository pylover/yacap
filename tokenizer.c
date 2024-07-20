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
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "option.h"
#include "tokenizer.h"


struct tokenizer {
    const struct optiondb *optiondb;
    int argc;
    const char **argv;

    /* tokenizer state */
    int line;
    int w;
    int c;
    int toklen;
    const char *tok;
    const struct optioninfo *optioninfo;
    bool dashdash;
};


/* Coroutine  stuff*/
#define YIELD_OPT(opt, v, l) do { \
        t->line = __LINE__; \
        token->text = v; \
        token->len = l; \
        token->optioninfo = opt; \
        return CARG_TOK_OPTION; \
        case __LINE__:; \
    } while (0)


#define YIELD_OPT_UNKNOWN(tok, l) do { \
        t->line = __LINE__; \
        token->text = tok; \
        token->len = l; \
        token->optioninfo = NULL; \
        return CARG_TOK_UNKNOWN; \
        case __LINE__:; \
    } while (0)


#define YIELD_POS(v, l) do { \
        t->line = __LINE__; \
        token->text = v; \
        token->len = l; \
        token->optioninfo = NULL; \
        return CARG_TOK_POSITIONAL; \
        case __LINE__:; \
    } while (0)


#define REJECT \
    t->line = -1; \
    token->text = NULL; \
    token->len = 0; \
    token->optioninfo = NULL; \
    return CARG_TOK_ERROR


#define END } \
    t->line = 0; \
    token->text = NULL; \
    token->len = 0; \
    token->optioninfo = NULL; \
    return CARG_TOK_END


#define START switch (t->line) { \
    case -1: REJECT; case 0:


struct tokenizer *
tokenizer_new(int argc, const char **argv,
        const struct optiondb *optdb) {
    struct tokenizer *t = malloc(sizeof(struct tokenizer));
    if (t == NULL) {
        return NULL;
    }

    t->line = 0;
    t->optiondb = optdb;
    t->argc = argc;
    t->argv = argv;
    t->dashdash = false;
    return t;
}


void
tokenizer_dispose(struct tokenizer *t) {
    if (t == NULL) {
        return;
    }

    free(t);
}


enum tokenizer_status
tokenizer_next(struct tokenizer *t, struct token *token) {
    const char *eq;

    START;
    for (t->w = 0; t->w < t->argc; t->w++) {
        t->tok = t->argv[t->w];
        t->optioninfo = NULL;

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

        /* double dashes */
        if ((t->tok[0] == '-') && (t->tok[1] == '-')) {
            /* threat the rest of tokens as positional arguments */
            if (t->toklen == 2) {
                t->dashdash = true;
                continue;
            }

            /* flag or option? '-foo' or '--foo=bar' */
            eq = strchr(t->tok, '=');

            /* Left side length */
            if ((t->toklen == 3) || (eq && ((eq - t->tok) == 3))) {
                YIELD_OPT_UNKNOWN(t->tok, t->toklen);
                continue;
            }

            // TODO: check the rightside len
            t->optioninfo = optiondb_findbyname(t->optiondb, t->tok + 2,
                    (eq? eq - t->tok: t->toklen) - 2);

            if (t->optioninfo == NULL) {
                YIELD_OPT_UNKNOWN(t->tok, t->toklen);
                continue;
            }

            if (!eq) {
                YIELD_OPT(t->optioninfo, NULL, 0);
                continue;
            }

            YIELD_OPT(t->optioninfo, eq+1, strlen(eq+1));
            continue;
        }

        if (t->tok[0] == '-') {
            /* Single dash option: -f */
            for (t->c = 1; t->c < t->toklen; t->c++) {
                t->optioninfo = optiondb_findbykey(t->optiondb, t->tok[t->c]);
                if (t->optioninfo == NULL) {
                    YIELD_OPT_UNKNOWN(t->tok + t->c, 1);
                    break;
                }
                else if (CARG_OPTION_ARGNEEDED(t->optioninfo->option) &&
                        ((t->c + 1) < t->toklen)) {
                    YIELD_OPT(t->optioninfo, t->tok + t->c + 1,
                            strlen(t->tok + t->c + 1));
                    break;
                }
                else {
                    YIELD_OPT(t->optioninfo, NULL, 0);
                }
            }

            continue;
        }

        /* Positional argument */
positional:
        YIELD_POS(t->tok, t->toklen);
    }

    END;
}
