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


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <clog.h>

#include "carg.h"


#define VERBOSITY_DEFAULT  CLOG_WARNING
#define HELP_LINESIZE 79
#define USAGE_BUFFSIZE 1024
#define TRYHELP(p) dprintf(_errfile, \
        "Try `%s --help' or `%s --usage' for more information.\n", p, p);
#define CMP(x, y, l) (strncmp(x, y, l) == 0)
#define MAX(x, y) ((x) > (y)? (x): (y))
#define BETWEEN(c, l, u) (((c) >= l) && ((c) <= u))
#define ISSIGN(c) (\
        BETWEEN(c, 32, 47) || \
        BETWEEN(c, 58, 64) || \
        BETWEEN(c, 123, 126))
#define ISDIGIT(c) BETWEEN(c, 48, 57)
#define ISCHAR(c) ((c == '?') || ISDIGIT(c) || \
        BETWEEN(c, 65, 90) || \
        BETWEEN(c, 97, 122))
#define HASFLAG(o, f) ((o)->flags & f)  // NOLINT
#define OPT_MINGAP   4
#define OPT_HELPLEN(o) ( \
    strlen((o)->name) + \
    ((o)->arg? strlen((o)->arg) + 1: 0) + \
    (HASFLAG(o, CARG_OPTIONAL_VALUE)? 2: 0))


static int _outfile = STDOUT_FILENO;
static int _errfile = STDERR_FILENO;
static struct carg_option opt_verbosity = {
    .name = "verbose",
    .key = 'v',
    .arg = "LEVEL",
    .flags = CARG_OPTIONAL_VALUE,
    .help = "Verbosity level. one of: '0|s|silent', '1|f|fatal', '2|e|error'"
        ", '3|w|warn', '4|i|info' and '5|d|debug'. if this option is not "
        "given, the verbosity level will be '3|w|warn', but If option is "
        "given without value, then the verbosity level will be '4|i|info'."
};
static struct carg_option opt_version = {"version", 'V', NULL, 0,
    "Print program version"};
static struct carg_option opt_help = {"help", 'h', NULL, 0,
    "Give this help list"};
static struct carg_option opt_usage = {"usage", '?', NULL, 0,
    "Give a short usage message"};


void
carg_outfile_set(int fd) {
    _outfile = fd;
}


void
carg_errfile_set(int fd) {
    _errfile = fd;
}


/* Options */
const struct carg_option *
_option_findbyname(const struct carg_option *options[], int count,
        const char *name, int len) {
    int i;
    const struct carg_option *opt;

    for (i = 0; i < count; i++) {
        opt = options[i];

        while (opt->name) {
            if (CMP(name, opt->name, len)) {
                return opt;
            }

            opt++;
        }
    }

    return NULL;
}


const struct carg_option *
_option_findbykey(const struct carg_option *options[], int count, int key) {
    int i;
    const struct carg_option *opt;

    for (i = 0; i < count; i++) {
        opt = options[i];

        while (opt->name) {
            if (opt->key == key) {
                return opt;
            }

            opt++;
        }
    }

    return NULL;
}


/* Tokenizer */
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
    int occurances[256];
    const struct carg_option *opt;
    const struct carg_option *opt2;
    bool dashdash;
};


struct carg_token {
    const char *value;
    const struct carg_option *option;
    int occurance;
};


/* Coroutine  stuff*/
#define YIELD_OPT(o, v) do { \
        t->line = __LINE__; \
        token->value = v; \
        token->option = o; \
        token->occurance = ++(t->occurances[(o)->key]); \
        return 1; \
        case __LINE__:; \
    } while (0)


#define YIELD_ARG(v) do { \
        t->line = __LINE__; \
        token->value = v; \
        token->option = NULL; \
        token->occurance = -1; \
        return 1; \
        case __LINE__:; \
    } while (0)


#define END } \
    t->line = 0; \
    token->value = NULL; \
    token->option = NULL; \
    token->occurance = -1; \
    return 0


#define REJECT \
    t->line = -1; \
    token->value = NULL; \
    token->option = NULL; \
    token->occurance = -1; \
    return -1


#define START switch (t->line) { case -1: REJECT; case 0:


static struct tokenizer *
_tokenizer_new(int argc, const char **argv,
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
    memset(t->occurances, 0, 255 * sizeof(int));
    return t;
}


static void
_tokenizer_dispose(struct tokenizer *t) {
    if (t == NULL) {
        return;
    }

    free(t);
}


static int
_tokenizer_next(struct tokenizer *t, struct carg_token *token) {
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
            t->opt = _option_findbyname(t->options, t->options_count,
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
                t->opt = _option_findbykey(t->options, t->options_count,
                        t->tok[t->c]);
                if (t->opt == NULL) {
                    YIELD_ARG(t->tok + (t->c == 1? 0: t->c));
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
        YIELD_ARG(t->tok);
    }

    END;
}


static int
_optionvectors(const struct carg *c, const struct carg_option **vects[]) {
    int count = 0;
    int i = 0;
    const struct carg_command **cmd = c->commands;
    const struct carg_option **v;

    if (c->options) {
        count++;
    }

    while (cmd++) {
        if (cmd[0]->options) {
            count++;
        }
    }

    v = malloc(count * (sizeof (struct carg_option*)));
    if (v == NULL) {
        return -1;
    }
    *vects = v;

    if (c->options) {
        v[i++] = c->options;
    }

    while (cmd++) {
        if (cmd[0]->options) {
            v[i++] = cmd[0]->options;
        }
    }

    return count;
}


int
carg_parse(const struct carg *c, int argc, const char **argv, void *userptr,
        void **handler) {
    const struct carg_option **optvects;
    int optvects_count = _optionvectors(c, &optvects);
    if (optvects_count < 0) {
        return -1;
    }

    struct tokenizer *t = _tokenizer_new(argc, argv, optvects,
            optvects_count);


    // while (true) {
    //     _tokenizer_next(t, &tok));
    // }


    // TODO: free optvects
}
