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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "helpers.h"
#include "builtin.h"
#include "state.h"
#include "help.h"


#define OPT_MINGAP 4
#define OPT_HELPLEN(o) ((o)->name? \
    (strlen((o)->name) + ((o)->arg? strlen((o)->arg) + 1: 0)): 0)


static int
_calculate_initial_gapsize(const struct yacap *c, bool subcommand) {
    int gapsize = 8;

#ifdef YACAP_USE_CLOG
    if ((!subcommand) && (!HASFLAG(c, YACAP_NO_CLOG))) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_verbosity) + OPT_MINGAP);
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_verboseflag) + OPT_MINGAP);
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_quietflag) + OPT_MINGAP);
    }
#endif

    if (!HASFLAG(c, YACAP_NO_HELP)) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_help) + OPT_MINGAP);
    }

    if (!HASFLAG(c, YACAP_NO_USAGE)) {
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
_print_optiongroup(int fd, const struct yacap_option *opt, int gapsize) {
    int rpad;

    if (opt->name && (!STREQ("-", opt->name))) {
        rpad = (gapsize + 8) - strlen(opt->name);
        dprintf(fd, "\n%s%*s", opt->name, rpad, "");
    }

    if (opt->help) {
        _print_multiline(fd, opt->help, gapsize + 8, YACAP_HELP_LINESIZE);
    }
    else {
        dprintf(fd, "\n");
    }
}


static void
_print_subcommands(int fd, const struct yacap_command *cmd) {
    struct yacap_command * const *c = cmd->commands;
    struct yacap_command *s;

    if (cmd->commands[0] == NULL) {
        return;
    }

    dprintf(fd, "\nCommands:\n");
    while ((s = *c)) {
        dprintf(fd, "  %s\n", s->name);
        c++;
    }
}


static void
_print_option(int fd, const struct yacap_option *opt, int gapsize) {
    int rpad = gapsize - OPT_HELPLEN(opt);

    if (ISCHAR(opt->key)) {
        dprintf(fd, "  -%c%c ", opt->key, opt->name? ',': ' ');
    }
    else {
        dprintf(fd, "      ");
    }

    if (opt->name) {
        if (opt->arg == NULL) {
            dprintf(fd, "--%s%*s", opt->name, rpad, "");
        }
        else {
            dprintf(fd, "--%s=%s%*s", opt->name, opt->arg, rpad, "");
        }
    }
    else {
        dprintf(fd, "  %*s", rpad, "");
    }

    if (opt->help) {
        _print_multiline(fd, opt->help, gapsize + 8, YACAP_HELP_LINESIZE);
    }
    else {
        dprintf(fd, "\n");
    }
}


static void
_print_options(int fd, const struct yacap *c, const struct yacap_command *cmd) {
    int gapsize;
    int i = 0;
    const struct yacap_option *opt;
    bool subcommand = c->state->cmdstack.len > 1;

    /* calculate gap size between options and description */
    gapsize = _calculate_initial_gapsize(c, subcommand);
    while (cmd->options) {
        opt = &(cmd->options[i++]);
        if (opt->name == NULL) {
            break;
        }

        gapsize = MAX(gapsize, OPT_HELPLEN(opt) + OPT_MINGAP);
    }

    dprintf(fd, "\nOptions:\n");
    if (!HASFLAG(c, YACAP_NO_HELP)) {
        _print_option(fd, &opt_help, gapsize);
    }

    if (!HASFLAG(c, YACAP_NO_USAGE)) {
        _print_option(fd, &opt_usage, gapsize);
    }

#ifdef YACAP_USE_CLOG
    if ((!subcommand) && (!HASFLAG(c, YACAP_NO_CLOG))) {
        _print_option(fd, &opt_verboseflag, gapsize);
        _print_option(fd, &opt_quietflag, gapsize);
        _print_option(fd, &opt_verbosity, gapsize);
    }
#endif

    if (!subcommand && c->version) {
        _print_option(fd, &opt_version, gapsize);
    }

    i = 0;
    while (cmd->options) {
        opt = &(cmd->options[i++]);
        if (opt->name == NULL) {
            break;
        }

        if (opt->key) {
            _print_option(fd, opt, gapsize);
        }
        else {
            _print_optiongroup(fd, opt, gapsize);
        }
    }
}


void
yacap_usage_print(const struct yacap *c) {
    char delim[1] = {'\n'};
    char *needle;
    char *saveptr = NULL;
    char *buff = NULL;
    struct yacap_state *state = c->state;
    const struct yacap_command *cmd = cmdstack_last(&state->cmdstack);

    POUT("Usage: ");
    cmdstack_print(STDOUT_FILENO, &state->cmdstack);
    POUT(" [OPTION...]");

    if (cmd->args == NULL) {
        goto done;
    }

    buff = malloc(strlen(cmd->args) + 1);
    strcpy(buff, cmd->args);

    needle = strtok_r(buff, delim, &saveptr);
    POUT(" %s", needle);
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        POUT("\n   or: ");
        cmdstack_print(STDOUT_FILENO, &state->cmdstack);
        POUT(" [OPTION...] %s", needle);
    }

done:
    if (buff) {
        free(buff);
    }
    POUT("\n");
}


void
yacap_help_print(const struct yacap *c) {
    struct yacap_state *state = c->state;
    const struct yacap_command *cmd = cmdstack_last(&state->cmdstack);

    /* usage */
    yacap_usage_print(c);

    /* header */
    if (cmd->header) {
        POUT("\n");
        _print_multiline(STDOUT_FILENO, cmd->header, 0, YACAP_HELP_LINESIZE);
    }

    /* sub-commands */
    if (cmd->commands[0]) {
        _print_subcommands(STDOUT_FILENO, cmd);
    }

    /* options */
    _print_options(STDOUT_FILENO, c, cmd);

    /* footer */
    if (cmd->footer) {
        POUT("\n");
        _print_multiline(STDOUT_FILENO, cmd->footer, 0, YACAP_HELP_LINESIZE);
    }
}
