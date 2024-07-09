#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <clog.h>

#include "config.h"
#include "option.h"
#include "tokenizer.h"


struct tokenizer {
    const struct carg_optiondb *optiondb;
    int argc;
    const char **argv;

    /* tokenizer state */
    int line;
    int w;
    int c;
    int toklen;
    const char *tok;
    int occurances[CARG_MAXOPTIONS];
    const struct carg_option *option;
    bool dashdash;
};


/* Coroutine  stuff*/
#define YIELD_OPT(opt, v) do { \
        t->line = __LINE__; \
        token->text = v; \
        token->len = 0; \
        token->option = opt; \
        token->occurance = ++(t->occurances[(opt)->key]); \
        return CARG_TOK_OPTION; \
        case __LINE__:; \
    } while (0)


#define YIELD_OPT_UNKNOWN(tok, l) do { \
        t->line = __LINE__; \
        token->text = tok; \
        token->len = l; \
        token->option = NULL; \
        token->occurance = -1; \
        return CARG_TOK_UNKNOWN; \
        case __LINE__:; \
    } while (0)


#define YIELD_POS(v) do { \
        t->line = __LINE__; \
        token->text = v; \
        token->len = 0; \
        token->option = NULL; \
        token->occurance = -1; \
        return CARG_TOK_POSITIONAL; \
        case __LINE__:; \
    } while (0)


#define REJECT \
    t->line = -1; \
    token->text = NULL; \
    token->len = 0; \
    token->option = NULL; \
    token->occurance = -1; \
    return CARG_TOK_ERROR


#define END } \
    t->line = 0; \
    token->text = NULL; \
    token->len = 0; \
    token->option = NULL; \
    token->occurance = -1; \
    return CARG_TOK_END


#define START switch (t->line) { case -1: REJECT; case 0:


struct tokenizer *
tokenizer_new(int argc, const char **argv,
        const struct carg_optiondb *optdb) {
    struct tokenizer *t = malloc(sizeof(struct tokenizer));
    if (t == NULL) {
        return NULL;
    }

    t->line = 0;
    t->optiondb = optdb;
    t->argc = argc;
    t->argv = argv;
    t->dashdash = false;
    memset(t->occurances, 0, sizeof(t->occurances));
    return t;
}


void
tokenizer_dispose(struct tokenizer *t) {
    if (t == NULL) {
        return;
    }

    free(t);
}


enum carg_tokenizer_status
tokenizer_next(struct tokenizer *t, struct carg_token *token) {
    const char *eq;

    START;
    for (t->w = 0; t->w < t->argc; t->w++) {
        t->tok = t->argv[t->w];
        t->option = NULL;

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
            t->option = optiondb_findbyname(t->optiondb, t->tok + 2,
                    (eq? eq - t->tok: t->toklen) - 2);

            if (t->option == NULL) {
                goto positional;
            }

            YIELD_OPT(t->option, eq? eq+1: NULL);
            continue;
        }

        if (t->tok[0] == '-') {
            /* Single dash option: -f */
            for (t->c = 1; t->c < t->toklen; t->c++) {
                t->option = optiondb_findbykey(t->optiondb, t->tok[t->c]);
                if (t->option == NULL) {
                    YIELD_OPT_UNKNOWN(t->tok + t->c, 1);
                    break;
                }
                else if (CARG_OPTION_ARGNEEDED(t->option) &&
                        ((t->c + 1) < t->toklen)) {
                    YIELD_OPT(t->option, t->tok + t->c + 1);
                    break;
                }
                else {
                    YIELD_OPT(t->option, NULL);
                }
            }

            continue;
        }

        /* Positional argument */
positional:
        YIELD_POS(t->tok);
    }

    END;
}
