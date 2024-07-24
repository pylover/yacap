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
#include "arghint.h"
#include "command.h"
#include "option.h"
#include "help.h"
#include "optiondb.h"
#include "tokenizer.h"


#define TRYHELP(s) \
    PERR("Try `"); \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(" --help' or `"); \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(" --usage' for more information.\n")

#define REJECT_OPTION_MISSINGARGUMENT(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": option requires an argument -- '"); \
    option_print(STDERR_FILENO, o); \
    PERR("'\n")

#define REJECT_OPTION_HASARGUMENT(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": no argument allowed for option -- '"); \
    option_print(STDERR_FILENO, o); \
    PERR("'\n")

#define REJECT_OPTION_UNRECOGNIZED(s, name, len) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": invalid option -- '%s%.*s'\n", \
        len == 1? "-": "", len, name)

#define REJECT_OPTION_NOTEATEN(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": option not eaten -- '"); \
    option_print(STDERR_FILENO, o); \
    PERR("'\n")

#define REJECT_OPTION_REDUNDANT(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": redundant option -- '"); \
    option_print(STDERR_FILENO, o); \
    PERR("'\n")

#define REJECT_POSITIONAL_NOTEATEN(s, t) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": argument not eaten -- '%s'\n", t)

#define REJECT_POSITIONAL(s, t) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": invalid argument -- '%s'\n", t)

#define REJECT_POSITIONALCOUNT(s) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": invalid arguments count\n")


static int
_build_optiondb(const struct carg *c, struct optiondb *db) {
    if (optiondb_init(db)) {
        return -1;
    }

    if (c->version && optiondb_insert(db, &opt_version,
                (struct carg_command *)c)) {
        return -1;
    }

    if ((!HASFLAG(c, CARG_NO_HELP)) && optiondb_insert(db, &opt_help,
                (struct carg_command *)c)) {
        return -1;
    }

    if ((!HASFLAG(c, CARG_NO_USAGE)) && optiondb_insert(db, &opt_usage,
                (struct carg_command *)c)) {
        return -1;
    }

#ifdef CARG_USE_CLOG
    if (!HASFLAG(c, CARG_NO_CLOG)) {
        if (optiondb_insert(db, &opt_verbosity, (struct carg_command *)c)) {
            return -1;
        }
        if (optiondb_insert(db, &opt_verboseflag, (struct carg_command *)c)) {
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
_eat(const struct carg *c, const struct carg_command *command,
        const struct carg_option *opt, const char *value) {
    /* Try to solve it internaly */
    if (c->version && (opt == &opt_version)) {
        POUT("%s\n", c->version);
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

    if (command->eat) {
        return command->eat(opt, value, command->userptr);
    }

    return CARG_EAT_NOTEATEN;
}


/* Helper macro */
#define NEXT(t, tok) tokenizer_next(t, tok)


static enum carg_status
_command_parse(struct carg *c, struct tokenizer *t) {
    enum carg_status status = CARG_OK;
    enum tokenizer_status tokstatus;
    enum carg_eatstatus eatstatus;
    struct token tok;
    struct token nexttok;
    struct carg_state *state = c->state;
    const struct carg_command *cmd = cmdstack_last(&state->cmdstack);
    const struct carg_command *subcmd;
    int positional_pattern = arghint_parse(cmd->args);

    if (optiondb_insertvector(&state->optiondb, cmd->options, cmd) == -1) {
        status = CARG_FATAL;
        goto terminate;
    }

    do {
        /* fetch the next token */
        if ((tokstatus = NEXT(t, &tok)) <= CARG_TOK_END) {
            if (tokstatus == CARG_TOK_UNKNOWN) {
                REJECT_OPTION_UNRECOGNIZED(state, tok.text, tok.len);
                status = CARG_USERERROR;
            }
            goto terminate;
        }

        /* is this a positional? */
        if (tok.optioninfo == NULL) {
            /* is this a sub-command? */
            subcmd = command_findbyname(cmd, tok.text);
            if (subcmd) {
                if (cmdstack_push(&state->cmdstack, tok.text, subcmd) == -1) {
                    status = CARG_FATAL;
                }
                else {
                    status = _command_parse(c, t);
                }
                goto terminate;
            }

            /* it's positional */
            state->positionals++;
            eatstatus = _eat(c, cmd, NULL, tok.text);
            goto dessert;
        }

        /* ensure option occureances */
        if ((!HASFLAG(tok.optioninfo->option, CARG_OPTION_MULTIPLE)) &&
                (tok.optioninfo->occurances > 1)) {
            REJECT_OPTION_REDUNDANT(state, tok.optioninfo->option);
            status = CARG_USERERROR;
            goto terminate;
        }


        /* ensure option's value */
        if (CARG_OPTION_ARGNEEDED(tok.optioninfo->option)) {
            if (tok.text == NULL) {
                /* try the next token as value */
                if ((tokstatus = NEXT(t, &nexttok))
                        != CARG_TOK_POSITIONAL) {
                    REJECT_OPTION_MISSINGARGUMENT(state,
                            tok.optioninfo->option);
                    status = CARG_USERERROR;
                    goto terminate;
                }

                tok.text = nexttok.text;
                tok.len = nexttok.len;
            }
            eatstatus = _eat(c, tok.optioninfo->command,
                    tok.optioninfo->option, tok.text);
        }
        else {
            if (tok.text) {
                REJECT_OPTION_HASARGUMENT(state, tok.optioninfo->option);
                status = CARG_USERERROR;
                goto terminate;
            }
            eatstatus = _eat(c, tok.optioninfo->command,
                    tok.optioninfo->option, NULL);
        }

dessert:
        switch (eatstatus) {
            case CARG_EAT_OK:
                continue;
            case CARG_EAT_OK_EXIT:
                status = CARG_OK_EXIT;
                goto terminate;
            case CARG_EAT_UNRECOGNIZED:
                REJECT_POSITIONAL(state, tok.text);
                status = CARG_USERERROR;
                goto terminate;
            case CARG_EAT_NOTEATEN:
                if (tok.optioninfo) {
                    REJECT_OPTION_NOTEATEN(state, tok.optioninfo->option);
                }
                else {
                    REJECT_POSITIONAL_NOTEATEN(state, tok.text);
                }
            default:
                status = CARG_FATAL;
                goto terminate;
        }
    } while (tokstatus > CARG_TOK_END);

    if (arghint_validate(positional_pattern, state->positionals)) {
        REJECT_POSITIONALCOUNT(state);
        status = CARG_USERERROR;
    }

terminate:
    return status;
}


enum carg_status
carg_parse(struct carg *c, int argc, const char **argv,
        const struct carg_subcommand **subcommand) {
    struct carg_state state = {.positionals = 0};
    enum carg_status status = CARG_OK;
    enum tokenizer_status tokstatus;
    struct token tok;
    struct tokenizer *t;

    if (argc < 1) {
        return CARG_FATAL;
    }

#ifdef CARG_USE_CLOG
    clog_verbosity = CLOG_WARNING;
#endif

    if (_build_optiondb(c, &state.optiondb)) {
        return CARG_FATAL;
    }

    t = tokenizer_new(argc, argv, &state.optiondb);
    if (t == NULL) {
        optiondb_dispose(&state.optiondb);
        return CARG_FATAL;
    }

    /* initialize command stack */
    cmdstack_init(&state.cmdstack);
    c->state = &state;

    /* excecutable name */
    if ((tokstatus = NEXT(t, &tok)) == CARG_TOK_POSITIONAL) {
        if (cmdstack_push(&state.cmdstack, tok.text,
                    (struct carg_command *)c) == -1) {
            goto terminate;
        }
    }
    else {
        goto terminate;
    }

    status = _command_parse(c, t);
    if (status < CARG_OK) {
        goto terminate;
    }

    if (subcommand) {
        if (state.cmdstack.len == 1) {
            /* root command */
            *subcommand = NULL;
        }
        else {
            /* sub-commands */
            *subcommand = (const struct carg_subcommand*)
                cmdstack_last(&state.cmdstack);
        }
    }

terminate:
    tokenizer_dispose(t);
    optiondb_dispose(&state.optiondb);
    if (status == CARG_USERERROR) {
        TRYHELP(&state);
    }
    c->state = NULL;
    return status;
}
