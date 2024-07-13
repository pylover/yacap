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
#include <ctype.h>

#include "config.h"
#include "carg.h"
#include "internal.h"
#include "option.h"
#include "help.h"
#include "optiondb.h"
#include "tokenizer.h"


#define TRYHELP(s) \
    dprintf(STDERR_FILENO, "Try `"); \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    dprintf(STDERR_FILENO, " --help' or `"); \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    dprintf(STDERR_FILENO, " --usage' for more information.\n")

#define REJECT_OPTIONMISSINGARGUMENT(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    dprintf(STDERR_FILENO, ": option requires an argument -- '"); \
    option_print(STDERR_FILENO, o); \
    dprintf(STDERR_FILENO, "'\n")

#define REJECT_OPTIONHASARGUMENT(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    dprintf(STDERR_FILENO, ": no argument allowed for option -- '"); \
    option_print(STDERR_FILENO, o); \
    dprintf(STDERR_FILENO, "'\n")

#define REJECT_UNRECOGNIZED(s, name, len) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    dprintf(STDERR_FILENO, ": invalid option -- '%s%.*s'\n", \
        len == 1? "-": "", len, name)


static int
_build_optiondb(const struct carg *c, struct optiondb *db) {
    int count = 0;
    const struct carg_command **cmd = c->commands;

    if (optiondb_init(db)) {
        return -1;
    }

    if (c->options) {
        count++;
    }

    while (cmd) {
        if ((*cmd)->options) {
            count++;
        }

        cmd++;
    }

    if (optiondb_insertvector(db, c->options) == -1) {
        return -1;
    }

    while (cmd) {
        if (optiondb_insertvector(db, (*(cmd++))->options) == -1) {
            return -1;
        }
    }

    if (c->version && optiondb_insert(db, &opt_version)) {
        return -1;
    }

    if ((!HASFLAG(c, CARG_NO_HELP)) && optiondb_insert(db, &opt_help)) {
        return -1;
    }

    if ((!HASFLAG(c, CARG_NO_USAGE)) && optiondb_insert(db, &opt_usage)) {
        return -1;
    }

#ifdef CARG_USE_CLOG
    if (!HASFLAG(c, CARG_NO_CLOG)) {
        if (optiondb_insert(db, &opt_verbosity)) {
            return -1;
        }
        if (optiondb_insert(db, &opt_verboseflag)) {
            return -1;
        }
    }
#endif

    return 0;
}


#ifdef CARG_USE_CLOG

#include <clog.h>


void
_clogverboser() {
    if (clog_verbosity < CLOG_DEBUG) {
        clog_verbosity++;
    }
}

void
_clogverbosity(const char *value) {
    int valuelen = value? strlen(value): 0;

    if (valuelen == 0) {
        clog_verbosity = CLOG_INFO;
        return;
    }

    if (valuelen == 1) {
        if (value[0] == 'v') {
            /* -vv */
            clog_verbosity = CLOG_DEBUG;
            return;
        }

        if (ISDIGIT(value[0])) {
            /* -v0 ... -v5 */
            clog_verbosity = atoi(value);
            if (!BETWEEN(clog_verbosity, CLOG_SILENT, CLOG_DEBUG)) {
                clog_verbosity = CLOG_INFO;
                return;
            }
            return;
        }
    }

    clog_verbosity = clog_verbosity_from_string(value);
    if (clog_verbosity == CLOG_UNKNOWN) {
        clog_verbosity = CLOG_INFO;
        return;
    }
}
#endif


static enum carg_eatstatus
_eat(const struct carg *c, const struct carg_option *opt,
        const char *value, void *userptr) {
    /* Try to solve it internaly */
    if (c->version && (opt == &opt_version)) {
        dprintf(STDOUT_FILENO, "%s\n", c->version);
        return CARG_EAT_OK_EXIT;
    }

    if ((!HASFLAG(c, CARG_NO_HELP)) && (opt == &opt_help)) {
        carg_help_print(c);
        return CARG_EAT_OK_EXIT;
    }

    if ((!HASFLAG(c, CARG_NO_USAGE)) && (opt == &opt_usage)) {
        carg_usage_print(c);
        return CARG_EAT_OK_EXIT;
    }

#ifdef CARG_USE_CLOG
    if (!HASFLAG(c, CARG_NO_CLOG)) {
        if (opt == &opt_verbosity) {
            _clogverbosity(value);
            return CARG_EAT_OK;
        }

        if (opt == &opt_verboseflag) {
            _clogverboser();
            return CARG_EAT_OK;
        }
    }
#endif

    if (c->eat) {
        return c->eat(opt, value, userptr);
    }

    return CARG_EAT_NOTEATEN;
}


enum carg_status
carg_parse(struct carg *c, int argc, const char **argv, void *userptr) {
    struct carg_state state;
    int status = CARG_OK;
    enum tokenizer_status tokstatus;
    enum carg_eatstatus eatstatus;
    struct optiondb optiondb;
    struct tokenizer *t;
    struct token tok;
    struct token nexttok;

    if (argc < 1) {
        return CARG_ERROR;
    }

#ifdef CARG_USE_CLOG
    clog_verbosity = CLOG_WARNING;
#endif

    if (_build_optiondb(c, &optiondb)) {
        return CARG_ERROR;
    }

    t = tokenizer_new(argc, argv, &optiondb);
    if (t == NULL) {
        optiondb_dispose(&optiondb);
        return CARG_ERROR;
    }

    /* initialize command stack */
    cmdstack_init(&state.cmdstack);

    /* Helper macro */
    #define NEXT(tok) tokenizer_next(t, tok)

    /* excecutable name */
    if ((tokstatus = NEXT(&tok)) == CARG_TOK_POSITIONAL) {
        if (cmdstack_push(&state.cmdstack, tok.text) == -1) {
            goto terminate;
        }
    }
    else {
        goto terminate;
    }
    c->state = &state;

    while (tokstatus > CARG_TOK_END) {
        /* fetch the next token */
        if ((tokstatus = NEXT(&tok)) <= CARG_TOK_END) {
            if (tokstatus == CARG_TOK_UNKNOWN) {
                REJECT_UNRECOGNIZED(&state, tok.text, tok.len);
                status = CARG_ERROR;
            }
            goto terminate;
        }

        /* is this a positional? */
        if (tok.option == NULL) {
            eatstatus = _eat(c, NULL, tok.text, userptr);
            goto dessert;
        }

        /* Ensure option's value */
        if (CARG_OPTION_ARGNEEDED(tok.option)) {
            if (tok.text == NULL) {
                /* try the next token as value */
                if ((tokstatus = NEXT(&nexttok)) != CARG_TOK_POSITIONAL) {
                    REJECT_OPTIONMISSINGARGUMENT(&state, tok.option);
                    status = CARG_ERROR;
                    goto terminate;
                }

                tok.text = nexttok.text;
                tok.len = nexttok.len;
            }
            eatstatus = _eat(c, tok.option, tok.text, userptr);
        }
        else {
            if (tok.text) {
                REJECT_OPTIONHASARGUMENT(&state, tok.option);
                status = CARG_ERROR;
                goto terminate;
            }
            eatstatus = _eat(c, tok.option, NULL, userptr);
        }

dessert:
        switch (eatstatus) {
            case CARG_EAT_OK:
                continue;
            case CARG_EAT_OK_EXIT:
                status = CARG_OK_EXIT;
                goto terminate;
            default:
                status = CARG_ERROR;
                goto terminate;
        }
    }

terminate:
    tokenizer_dispose(t);
    optiondb_dispose(&optiondb);
    if (status < 0) {
        TRYHELP(&state);
    }
    return status;
}
