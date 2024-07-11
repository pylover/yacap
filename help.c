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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "common.h"
#include "help.h"
#include "state.h"


#define OPT_MINGAP 4
#define OPT_HELPLEN(o) ( \
    strlen((o)->name) + \
    ((o)->arg? strlen((o)->arg) + 1: 0))


static int
_calculate_initial_gapsize(const struct carg *c) {
    int gapsize = 8;

    if (!HASFLAG(c, CARG_NO_CLOG)) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_verbosity) + OPT_MINGAP);
    }

    if (!HASFLAG(c, CARG_NO_HELP)) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_help) + OPT_MINGAP);
    }

    if (!HASFLAG(c, CARG_NO_USAGE)) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_usage) + OPT_MINGAP);
    }

    if (c->version) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_version) + OPT_MINGAP);
    }

    return gapsize;
}


static void
_print_multiline(int fd, const char *string, int indent, int linemax) {
    int remain;
    int linesize = linemax - indent;
    int ls;
    bool dash = false;

    if (string == NULL) {
        return;
    }

    remain = strlen(string);
    while (remain) {
        dash = false;
        while (remain && isspace(string[0])) {
            string++;
            remain--;
        }

        if (remain <= linesize) {
            dprintf(fd, "%s\n", string);
            remain = 0;
            break;
        }

        ls = linesize;
        if (string[ls - 2] == ' ') {
            ls--;
        }
        else if ((string[ls - 1] != ' ') && (string[ls] != ' ') &&
                (!ISSIGN(string[ls - 1])) && (!ISSIGN(string[ls]))) {
            ls--;
            dash = true;
        }

        if (string[ls - 1] == ' ') {
            ls--;
        }

        dprintf(fd, "%.*s%s\n", ls, string, dash? "-": "");
        remain -= ls;
        string += ls;
        dprintf(fd, "%*s", indent, "");
    }
}


static void
_print_option(int fd, const struct carg_option *opt, int gapsize) {
    int rpad = gapsize - OPT_HELPLEN(opt);

    if (ISCHAR(opt->key)) {
        dprintf(fd, "  -%c, ", opt->key);
    }
    else {
        dprintf(fd, "      ");
    }

    if (opt->arg == NULL) {
        dprintf(fd, "--%s%*s", opt->name, rpad, "");
    }
    else {
        dprintf(fd, "--%s=%s%*s", opt->name, opt->arg, rpad, "");
    }

    if (opt->help) {
        _print_multiline(fd, opt->help, gapsize + 8, CARG_HELP_LINESIZE);
    }
    else {
        dprintf(fd, "\n");
    }
}


static void
_print_options(int fd, const struct carg *c) {
    int gapsize;
    int i = 0;
    const struct carg_option *opt;

    /* calculate initial gap between options and description */
    gapsize = _calculate_initial_gapsize(c);

    while (c->options) {
        opt = &(c->options[i++]);
        if (opt->name == NULL) {
            break;
        }

        gapsize = MAX(gapsize, OPT_HELPLEN(opt) + OPT_MINGAP);
    }

    dprintf(fd, "\n");
    i = 0;
    while (c->options) {
        opt = &(c->options[i++]);
        if (opt->name == NULL) {
            break;
        }

        _print_option(fd, opt, gapsize);
    }

    if (!HASFLAG(c, CARG_NO_HELP)) {
        _print_option(fd, &opt_help, gapsize);
    }

    if (!HASFLAG(c, CARG_NO_USAGE)) {
        _print_option(fd, &opt_usage, gapsize);
    }

    if (!HASFLAG(c, CARG_NO_CLOG)) {
        _print_option(fd, &opt_verbosity, gapsize);
    }

    if (c->version) {
        _print_option(fd, &opt_version, gapsize);
    }
}


void
carg_usage_print(const struct carg *c) {
    char delim[1] = {'\n'};
    char *needle;
    char *saveptr = NULL;
    char *buff = NULL;
    struct carg_state *state = c->state;

    dprintf(STDOUT_FILENO, "Usage: %s [OPTION...]", state->prog);
    if (c->args == NULL) {
        goto done;
    }

    buff = malloc(strlen(c->args) + 1);
    strcpy(buff, c->args);

    needle = strtok_r(buff, delim, &saveptr);
    dprintf(STDOUT_FILENO, " %s", needle);
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        dprintf(STDOUT_FILENO, "\n   or: %s [OPTION...] %s", state->prog,
                needle);
    }

done:
    if (buff) {
        free(buff);
    }
    dprintf(STDOUT_FILENO, "\n");
}


void
carg_help_print(const struct carg *c) {
    // /* Usage */
    carg_usage_print(c);

    /* Header */
    if (c->header) {
        dprintf(STDOUT_FILENO, "\n");
        _print_multiline(STDOUT_FILENO, c->header, 0, CARG_HELP_LINESIZE);
    }

    /* Options */
    _print_options(STDOUT_FILENO, c);

    /* Footer */
    if (c->footer) {
        dprintf(STDOUT_FILENO, "\n");
        _print_multiline(STDOUT_FILENO, c->footer, 0, CARG_HELP_LINESIZE);
    }
}