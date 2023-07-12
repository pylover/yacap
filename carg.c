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


#define VERBOSITY_DEFAULT  CLOG_WARNING
#define HELP_LINESIZE 79
#define USAGE_BUFFSIZE 1024
#define TRYHELP(p) dprintf(_errfile, \
        "Try `%s --help' or `%s --usage' for more information.\n", p, p);
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

    struct tokenizer *t = tokenizer_new(argc, argv, optvects,
            optvects_count);


    // while (true) {
    //     tokenizer_next(t, &tok));
    // }


    // TODO: free optvects
    // TODO: free tokenizer
}
