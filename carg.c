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
#include "tokenizer.h"


#define TRYHELP(p) ERRORH( \
        "Try `%s --help' or `%s --usage' for more information.", p, p);
/*
#define REJECT_UNRECOGNIZED(name, v) \
    ERROR("%s: %s: (PARSE ERROR) Option should have been recognized!?", \
            t->prog, name)

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
_optionvectors(const struct carg *c, const struct carg_option **opts[]) {
    int count = 0;
    int i = 0;
    const struct carg_command **cmd = c->commands;
    const struct carg_option **v;

    if (c->options) {
        count++;
    }

    while (cmd) {
        if ((*cmd)->options) {
            count++;
        }

        cmd++;
    }

    v = malloc(count * (sizeof (struct carg_option*)));
    if (v == NULL) {
        return -1;
    }
    *opts = v;

    if (c->options) {
        v[i++] = c->options;
    }

    while (cmd) {
        if ((*cmd)->options) {
            v[i++] = cmd[0]->options;
        }

        cmd++;
    }

    return count;
}


#define CARG_VALUENEEDED(opt) ((opt)->arg != NULL)


enum carg_status
carg_parse(const struct carg *c, int argc, const char **argv, void *userptr) {
    int status;
    const char *prog;
    const struct carg_option **options;
    int optvects_count;
    struct tokenizer *t;
    struct carg_token tok;

    if (argc <= 1) {
        return CARG_ERROR;
    }

    optvects_count = _optionvectors(c, &options);
    if (optvects_count < 0) {
        return -1;
    }

    t = tokenizer_new(argc, argv, options, optvects_count);
    if (t == NULL) {
        status = -1;
        goto terminate;
    }

    /* excecutable name */
    if ((status = tokenizer_next(t, &tok)) == 1) {
        prog = tok.text;
    }

    while (status == 1) {
        /* fetch the next token */
        if ((status = tokenizer_next(t, &tok)) <= 0) {
            break;
        }

        if (tok.option == NULL) {

        }

        /* Ensure option's value */
        if (tok.option && CARG_VALUENEEDED(tok.option)) {

        }

    }

    if (status < 0) {
        TRYHELP(argv[0]);
    }

terminate:
    tokenizer_dispose(t);
    free(options);
    if (status < 0) {
        return CARG_ERROR;
    }
    return CARG_OK;
}
