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

#include <clog.h>

#include "carg.h"
#include "common.h"
#include "option.h"
#include "help.h"
#include "optiondb.h"
#include "tokenizer.h"
#include "state.h"


#define TRYHELP(p) ERRORH( \
        "Try `%s --help' or `%s --usage' for more information.", p, p)

#define REJECT_OPTIONMISSINGARGUMENT(p, o) dprintf(STDERR_FILENO, \
        "%s: option requires an argument -- '%s'\n", p, option_repr(o))

#define REJECT_OPTIONHASARGUMENT(p, o) dprintf(STDERR_FILENO, \
        "%s: option not requires any argument(s) -- '%s'\n", p, \
        option_repr(o))

#define REJECT_UNRECOGNIZED(p, name, len) dprintf(STDERR_FILENO, \
        "%s: invalid option -- '%s%.*s'\n", p, len == 1? "-": "", len, name)


static int
_build_optiondb(const struct carg *c, struct carg_optiondb *db) {
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

    return 0;
}


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
        return CARG_OK_EXIT;
    }

    if ((!HASFLAG(c, CARG_NO_USAGE)) && (opt == &opt_usage)) {
        carg_usage_print(c);
        return CARG_OK_EXIT;
    }

    if (c->eat) {
        return c->eat(opt, value, userptr);
    }

    return CARG_EAT_NOTEATEN;
}


enum carg_status
carg_parse(struct carg *c, int argc, const char **argv, void *userptr) {
    struct carg_state state;
    int status = CARG_OK;
    enum carg_tokenizer_status tokstatus;
    enum carg_eatstatus eatstatus;
    struct carg_optiondb optiondb;
    struct tokenizer *t;
    struct carg_token tok;
    struct carg_token nexttok;

    if (argc <= 1) {
        return CARG_ERROR;
    }

    if (_build_optiondb(c, &optiondb)) {
        return CARG_ERROR;
    }

    t = tokenizer_new(argc, argv, &optiondb);
    if (t == NULL) {
        optiondb_dispose(&optiondb);
        return CARG_ERROR;
    }

    /* Helper macro */
    #define NEXT(tok) tokenizer_next(t, tok)

    /* excecutable name */
    if ((tokstatus = NEXT(&tok)) == CARG_TOK_POSITIONAL) {
        state.prog = tok.text;
    }
    else {
        goto terminate;
    }

    c->state = &state;

    while (tokstatus > CARG_TOK_END) {
        /* fetch the next token */
        if ((tokstatus = NEXT(&tok)) <= CARG_TOK_END) {
            if (tokstatus == CARG_TOK_UNKNOWN) {
                REJECT_UNRECOGNIZED(state.prog, tok.text, tok.len);
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
                    REJECT_OPTIONMISSINGARGUMENT(state.prog, tok.option);
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
                REJECT_OPTIONHASARGUMENT(state.prog, tok.option);
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
        TRYHELP(state.prog);
    }
    return status;
}
