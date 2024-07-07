#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <clog.h>

#include "config.h"
#include "tokenizer.h"
#include "option.h"


struct tokenizer {
    const struct carg_option **options;
    int options_count;
    int argc;
    const char **argv;

    /* tokenizer state */
    int line;
    int w;
    int c;
    int toklen;
    const char *tok;
    int occurances[CARG_MAXOPTIONS];
    const struct carg_option *opt;
    const struct carg_option *opt2;
    bool dashdash;
};


/* Coroutine  stuff*/
#define YIELD_OPT(opt, v) do { \
        t->line = __LINE__; \
        token->text = v; \
        token->option = opt; \
        token->occurance = ++(t->occurances[(opt)->key]); \
        return 1; \
        case __LINE__:; \
    } while (0)


#define YIELD_POS(v) do { \
        t->line = __LINE__; \
        token->text = v; \
        token->option = NULL; \
        token->occurance = -1; \
        return 1; \
        case __LINE__:; \
    } while (0)


#define REJECT \
    t->line = -1; \
    token->text = NULL; \
    token->option = NULL; \
    token->occurance = -1; \
    return -1


#define END } \
    t->line = 0; \
    token->text = NULL; \
    token->option = NULL; \
    token->occurance = -1; \
    return 0


#define START switch (t->line) { case -1: REJECT; case 0:


struct tokenizer *
tokenizer_new(int argc, const char **argv,
        const struct carg_option *options[], int count) {
    struct tokenizer *t = malloc(sizeof(struct tokenizer));
    if (t == NULL) {
        return NULL;
    }

    t->line = 0;
    t->options = options;
    t->options_count = count;
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

        /* double dashes */
        if ((t->tok[0] == '-') && (t->tok[1] == '-')) {
            /* threat the rest of tokens as positional arguments */
            if (t->toklen == 2) {
                t->dashdash = true;
                continue;
            }

            /* flag or option? '-foo' or '--foo=bar' */
            eq = strchr(t->tok, '=');
            t->opt = option_findbyname(t->options, t->options_count,
                    t->tok + 2, (eq? eq - t->tok: t->toklen) - 2);

            if (t->opt == NULL) {
                goto positional;
            }

            YIELD_OPT(t->opt, eq? eq+1: NULL);
            continue;
        }

        if (t->tok[0] == '-') {
            /* Single dash option: -f */
            for (t->c = 1; t->c < t->toklen; t->c++) {
                t->opt = option_findbykey(t->options, t->options_count,
                        t->tok[t->c]);
                if (t->opt == NULL) {
                    YIELD_POS(t->tok + (t->c == 1? 0: t->c));
                    break;
                }
                else if (t->opt->arg && ((t->c + 1) < t->toklen)) {
                    YIELD_OPT(t->opt, t->tok + t->c + 1);
                    break;
                }
                else {
                    YIELD_OPT(t->opt, NULL);
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
