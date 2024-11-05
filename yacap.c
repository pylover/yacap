// Copyright 2023 Vahid Mardani
/*
 * This file is part of yacap.
 *  yacap is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  yacap is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with yacap. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "config.h"
#include "yacap.h"
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
    PERR(": invalid positional arguments count\n")


static int
_build_optiondb(const struct yacap *c, struct optiondb *db) {
    if (optiondb_init(db)) {
        return -1;
    }

    if (c->version && optiondb_insert(db, &opt_version,
                (struct yacap_command *)c)) {
        return -1;
    }

    if ((!HASFLAG(c, YACAP_NO_HELP)) && optiondb_insert(db, &opt_help,
                (struct yacap_command *)c)) {
        return -1;
    }

    if ((!HASFLAG(c, YACAP_NO_USAGE)) && optiondb_insert(db, &opt_usage,
                (struct yacap_command *)c)) {
        return -1;
    }

#ifdef YACAP_USE_CLOG
    if (!HASFLAG(c, YACAP_NO_CLOG)) {
        if (optiondb_insert(db, &opt_verbosity, (struct yacap_command *)c)) {
            return -1;
        }
        if (optiondb_insert(db, &opt_verboseflag, (struct yacap_command *)c)) {
            return -1;
        }
    }
#endif

    return 0;
}


#ifdef YACAP_USE_CLOG

#include <clog.h>


void
_clogverboser() {
    if (clog_verbosity < CLOG_DEBUG2) {
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
        if (ISDIGIT(value[0])) {
            /* -v0 ... -v5 */
            clog_verbosity = atoi(value);
            if (!BETWEEN(clog_verbosity, CLOG_SILENT, CLOG_DEBUG2)) {
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


static enum yacap_eatstatus
_eat(const struct yacap *c, const struct yacap_command *command,
        const struct yacap_option *opt, const char *value) {
    /* Try to solve it internaly */
    if (c->version && (opt == &opt_version)) {
        POUT("%s\n", c->version);
        return YACAP_EAT_OK_EXIT;
    }

    if ((!HASFLAG(c, YACAP_NO_HELP)) && (opt == &opt_help)) {
        yacap_help_print(c);
        return YACAP_EAT_OK_EXIT;
    }

    if ((!HASFLAG(c, YACAP_NO_USAGE)) && (opt == &opt_usage)) {
        yacap_usage_print(c);
        return YACAP_EAT_OK_EXIT;
    }

#ifdef YACAP_USE_CLOG
    if (!HASFLAG(c, YACAP_NO_CLOG)) {
        if (opt == &opt_verbosity) {
            _clogverbosity(value);
            return YACAP_EAT_OK;
        }

        if (opt == &opt_verboseflag) {
            _clogverboser();
            return YACAP_EAT_OK;
        }
    }
#endif

    if (command->eat) {
        return command->eat(opt, value, command->userptr);
    }

    return YACAP_EAT_NOTEATEN;
}


/* Helper macro */
#define NEXT(t, tok) tokenizer_next(t, tok)


static enum yacap_status
_command_parse(struct yacap *c, struct tokenizer *t) {
    enum yacap_status status = YACAP_OK;
    enum tokenizer_status tokstatus;
    enum yacap_eatstatus eatstatus;
    struct token tok;
    struct token nexttok;
    struct yacap_state *state = c->state;
    const struct yacap_command *cmd = cmdstack_last(&state->cmdstack);
    const struct yacap_command *subcmd = NULL;
    int arghint = arghint_parse(cmd->args);

    if (optiondb_insertvector(&state->optiondb, cmd->options, cmd) == -1) {
        status = YACAP_FATAL;
        goto terminate;
    }

    do {
        /* fetch the next token */
        if ((tokstatus = NEXT(t, &tok)) <= YACAP_TOK_END) {
            if (tokstatus == YACAP_TOK_UNKNOWN) {
                REJECT_OPTION_UNRECOGNIZED(state, tok.text, tok.len);
                status = YACAP_USERERROR;
            }
            goto terminate;
        }

        /* is this a positional? */
        if (tok.optioninfo == NULL) {
            /* is this a sub-command? */
            subcmd = command_findbyname(cmd, tok.text);
            if (subcmd) {
                if (cmdstack_push(&state->cmdstack, tok.text, subcmd) == -1) {
                    status = YACAP_FATAL;
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
        if ((!HASFLAG(tok.optioninfo->option, YACAP_OPTION_MULTIPLE)) &&
                (tok.optioninfo->occurances > 1)) {
            REJECT_OPTION_REDUNDANT(state, tok.optioninfo->option);
            status = YACAP_USERERROR;
            goto terminate;
        }


        /* ensure option's value */
        if (YACAP_OPTION_ARGNEEDED(tok.optioninfo->option)) {
            if (tok.text == NULL) {
                /* try the next token as value */
                if ((tokstatus = NEXT(t, &nexttok))
                        != YACAP_TOK_POSITIONAL) {
                    REJECT_OPTION_MISSINGARGUMENT(state,
                            tok.optioninfo->option);
                    status = YACAP_USERERROR;
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
                status = YACAP_USERERROR;
                goto terminate;
            }
            eatstatus = _eat(c, tok.optioninfo->command,
                    tok.optioninfo->option, NULL);
        }

dessert:
        switch (eatstatus) {
            case YACAP_EAT_OK:
                continue;
            case YACAP_EAT_OK_EXIT:
                status = YACAP_OK_EXIT;
                goto terminate;
            case YACAP_EAT_UNRECOGNIZED:
                REJECT_POSITIONAL(state, tok.text);
                status = YACAP_USERERROR;
                goto terminate;
            case YACAP_EAT_NOTEATEN:
                if (tok.optioninfo) {
                    REJECT_OPTION_NOTEATEN(state, tok.optioninfo->option);
                }
                else {
                    REJECT_POSITIONAL_NOTEATEN(state, tok.text);
                }
            default:
                status = YACAP_FATAL;
                goto terminate;
        }
    } while (tokstatus > YACAP_TOK_END);

terminate:
    if ((status == YACAP_OK) && (subcmd == NULL) &&
            arghint_validate(state->positionals, arghint)) {
        REJECT_POSITIONALCOUNT(state);
        status = YACAP_USERERROR;
    }

    return status;
}


enum yacap_status
yacap_parse(struct yacap *c, int argc, const char **argv,
        const struct yacap_command **command) {
    struct yacap_state *state;
    enum yacap_status status = YACAP_OK;
    enum tokenizer_status tokstatus;
    struct token tok;
    struct tokenizer *t;

    if (argc < 1) {
        return YACAP_FATAL;
    }

    state = malloc(sizeof(struct yacap_state));
    if (state == NULL) {
        return YACAP_FATAL;
    }
    memset(state, 0, sizeof(struct yacap_state));
    state->positionals = 0;
    c->state = state;

    if (_build_optiondb(c, &state->optiondb)) {
        return YACAP_FATAL;
    }

    t = tokenizer_new(argc, argv, &state->optiondb);
    if (t == NULL) {
        optiondb_dispose(&state->optiondb);
        return YACAP_FATAL;
    }

    /* initialize command stack */
    cmdstack_init(&state->cmdstack);

    /* excecutable name */
    if ((tokstatus = NEXT(t, &tok)) != YACAP_TOK_POSITIONAL) {
        goto terminate;
    }

    if (cmdstack_push(&state->cmdstack, tok.text,
                (struct yacap_command *)c) == -1) {
        goto terminate;
    }
    c->name = tok.text;

    status = _command_parse(c, t);
    if (status < YACAP_OK) {
        goto terminate;
    }

    /* commands */
    if (command) {
        *command = cmdstack_last(&state->cmdstack);
    }

terminate:
    tokenizer_dispose(t);
    optiondb_dispose(&state->optiondb);
    if (status == YACAP_USERERROR) {
        TRYHELP(state);
    }
    return status;
}


int
yacap_dispose(struct yacap *c) {
    if (c == NULL) {
        return -1;
    }

    if (c->state == NULL) {
        return -1;
    }

    free(c->state);
    c->state = NULL;
    return 0;
}


int
yacap_try_help(const struct yacap* c) {
    if (c == NULL) {
        return -1;
    }

    if (c->state == NULL) {
        return -1;
    }

    TRYHELP(c->state);
    return 0;
}


int
yacap_commandchain_print(int fd, const struct yacap *c) {
    if (fd < 0) {
        return -1;
    }

    if (c == NULL) {
        return -1;
    }

    return cmdstack_print(fd, &c->state->cmdstack);
}
