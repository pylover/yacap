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
#include "option.h"
#include "optiondb.h"
#include "tokenizer.h"


#define TRYHELP(p) ERRORH( \
        "Try `%s --help' or `%s --usage' for more information.", p, p)

#define REJECT_OPTIONMISSINGARGUMENT(p, o) dprintf(STDERR_FILENO, \
        "%s: option requires an argument -- '%s'\n", p, option_repr(o))

#define REJECT_UNRECOGNIZED(p, name, len) dprintf(STDERR_FILENO, \
        "%s: invalid option -- '%s%.*s'\n", p, len == 1? "-": "", len, name)

/*
#define VERBOSITY_DEFAULT  CLOG_WARNING
#define HELP_LINESIZE 79
#define USAGE_BUFFSIZE 1024
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
*/

/*
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
*/


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

    optiondb_insert(db, c->options);
    while (cmd) {
        optiondb_insert(db, (*(cmd++))->options);
    }

    return 0;
}


enum carg_status
carg_parse(const struct carg *c, int argc, const char **argv, void *userptr) {
    const char *prog;
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
        prog = tok.text;
    }
    else {
        goto terminate;
    }

    while (tokstatus > CARG_TOK_END) {
        /* fetch the next token */
        if ((tokstatus = NEXT(&tok)) <= CARG_TOK_END) {
            if (tokstatus == CARG_TOK_UNKNOWN) {
                REJECT_UNRECOGNIZED(prog, tok.text, tok.len);
                status = CARG_ERROR;
            }
            goto terminate;
        }

        /* is this a positional? */
        if (tok.option == NULL) {
            eatstatus = c->eat(NULL, tok.text, userptr);
            goto dessert;
        }

        /* Ensure option's value */
        if (CARG_OPTION_ARGNEEDED(tok.option)) {
            if (tok.text == NULL) {
                /* try the next token as value */
                if ((tokstatus = NEXT(&nexttok)) != CARG_TOK_POSITIONAL) {
                    REJECT_OPTIONMISSINGARGUMENT(prog, tok.option);
                    status = CARG_ERROR;
                    goto terminate;
                }

                tok.text = nexttok.text;
                tok.len = nexttok.len;
            }
            eatstatus = c->eat(tok.option, tok.text, userptr);
        }
        else {
            eatstatus = c->eat(tok.option, NULL, userptr);
        }

dessert:
        switch (eatstatus) {
            case CARG_EAT_OK:
                continue;
            case CARG_EAT_UNRECOGNIZED:
                status = CARG_ERROR;
            case CARG_EAT_OK_EXIT:
                goto terminate;
        }
    }

terminate:
    tokenizer_dispose(t);
    optiondb_dispose(&optiondb);
    if (status < 0) {
        TRYHELP(prog);
        return CARG_ERROR;
    }
    return CARG_OK;
}
