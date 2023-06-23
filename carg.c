// Copyright 2023 Vahid Mardani
/*
 * This file is part of Carrow.
 *  Carrow is free software: you can redistribute it and/or modify it under 
 *  the terms of the GNU General Public License as published by the Free 
 *  Software Foundation, either version 3 of the License, or (at your option) 
 *  any later version.
 *  
 *  Carrow is distributed in the hope that it will be useful, but WITHOUT ANY 
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 *  details.
 *  
 *  You should have received a copy of the GNU General Public License along 
 *  with Carrow. If not, see <https://www.gnu.org/licenses/>. 
 *  
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <clog.h>

#include "carg.h"


static int _outfile = STDOUT_FILENO;
static int _errfile = STDERR_FILENO;
static struct carg_option opt_version = {"version", 'V', NULL, 0,
    "Print program version"};
static struct carg_option opt_help = {"help", 'h', NULL, 0,
    "Give this help list"};
static struct carg_option opt_usage = {"usage", '?', NULL, 0,
    "Give a short usage message"};


#define HELP_LINESIZE 79
#define USAGE_BUFFSIZE 1024
#define TRYHELP(p) dprintf(_errfile, \
        "Try `%s --help' or `%s --usage' for more information.\n", p, p);
#define CMP(x, y, l) (strncmp(x, y, l) == 0)
#define MAX(x, y) ((x) > (y)? (x): (y))
#define BETWEEN(c, l, u) (((c) >= l) && ((c) <= u))
#define ISCHAR(c) ((c == '?') || \
        BETWEEN(c, 48, 57) || \
        BETWEEN(c, 65, 90) || \
        BETWEEN(c, 97, 122))


enum carg_argtype {
    CAT_COMMAND,
    CAT_SHORT,
    CAT_LONG,
};


void
carg_outfile_set(int fd) {
    _outfile = fd;
}


void
carg_errfile_set(int fd) {
    _errfile = fd;
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
        else if ((string[ls - 1] != ' ') && (string[ls] != ' ')) {
            ls--;
            dash = true;
        }

        dprintf(fd, "%.*s%s\n", ls, string, dash? "-": "");
        remain -= ls;
        string += ls;
        dprintf(fd, "%*s", indent, "");
    }
}


static void
_print_options(int fd, struct carg *c) {
    int gapsize = 7;
    int i = 0;
    struct carg_option *opt;
    int tmp = 0;

    while (true) {
        opt = &(c->options[i++]);

        if (opt->longname == NULL) {
            break;
        }

        tmp = strlen(opt->longname) + (opt->arg? strlen(opt->arg) + 1: 0);
        gapsize = MAX(gapsize, tmp);
    }
    gapsize += 8;
    char gap[gapsize + 1];
    gap[gapsize] = '\0';
    memset(gap, ' ', gapsize);

    dprintf(fd, "\n");
    i = 0;
    while (true) {
        opt = &(c->options[i++]);

        if (opt->longname == NULL) {
            break;
        }

        if (ISCHAR(opt->key)) {
            dprintf(fd, "  -%c, ", opt->key);
        }
        else {
            dprintf(fd, "      ");
        }

        if (opt->arg == NULL) {
            dprintf(fd, "--%s%.*s", opt->longname,
                    gapsize - ((int)strlen(opt->longname)), gap);
        }
        else {
            tmp = gapsize -
                (int)(strlen(opt->longname) + strlen(opt->arg) + 1);
            dprintf(fd, "--%s=%s%.*s", opt->longname, opt->arg, tmp, gap);
        }

        if (opt->help) {
            _print_multiline(fd, opt->help, gapsize + 8, HELP_LINESIZE);
        }
        else {
            dprintf(fd, "\n");
        }
    }
    dprintf(fd, "  -h, --help%.*sGive this help list\n",
            gapsize - 4, gap);
    dprintf(fd, "  -?, --usage%.*sGive a short usage message\n",
            gapsize - 5, gap);

    if (c->version) {
        dprintf(fd, "  -V, --version%.*sPrint program version\n",
                gapsize - 7, gap);
    }

    dprintf(fd, "\n");
}


void
carg_print_usage(struct carg_state *state) {
    char delim[1] = {'\n'};
    char *needle;
    char *saveptr = NULL;

    dprintf(state->fd, "Usage: %s [OPTION...]", state->argv[0]);
    if (state->carg->args == NULL) {
        goto done;
    }

    static char buff[USAGE_BUFFSIZE];
    strcpy(buff, state->carg->args);

    needle = strtok_r(buff, delim, &saveptr);
    dprintf(state->fd, " %s", needle);
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        dprintf(state->fd, "\n   or: %s [OPTION...] %s", state->argv[0],
                needle);
    }

done:
    dprintf(state->fd, "\n");
}


void
carg_print_help(struct carg_state *state) {
    /* Usage */
    carg_print_usage(state);

    /* Header */
    _print_multiline(state->fd, state->carg->header, 0, HELP_LINESIZE);

    /* Options */
    _print_options(state->fd, state->carg);

    /* Footer */
    _print_multiline(state->fd, state->carg->footer, 0, HELP_LINESIZE);
}


static void
_unrecognized_option(struct carg_state *state) {
    char *prog = state->argv[0];
    dprintf(_errfile, "%s: unrecognized option '%s'\n", prog,
            state->argv[state->current]);
    TRYHELP(prog);
}


static void
_not_eaten(struct carg_state *state, struct carg_option *opt) {
    char *prog = state->argv[0];
    const char *c = state->argv[state->current];

    if (opt) {
        dprintf(_errfile, "%s: -%c: (PROGRAM ERROR) "
                "Option should have been recognized!?\n", prog, opt->key);
    }
    else if (c[0] == '-') {
        _unrecognized_option(state);
        return;
    }
    else {
        dprintf(_errfile, "%s: Invalid argument: %s\n", prog, c);
    }
    TRYHELP(prog);
}


static void
_value_required(struct carg_state *state) {
    char *prog = state->argv[0];
    dprintf(_errfile, "%s: '%s' option requires an argument\n", prog,
            state->argv[state->current]);
    TRYHELP(prog);
}


static void
_arg_insufficient(struct carg_state *state) {
    char *prog = state->argv[0];
    dprintf(_errfile, "%s: insufficient argument(s)\n", prog);
    TRYHELP(prog);
}


static struct carg_option *
_option_bykey(struct carg_option *opt, const char user) {
    switch (user) {
        case 'h':
            return &opt_help;
        case 'V':
            return &opt_version;
        case '?':
            return &opt_usage;
    }

    while (opt->longname) {
        if (opt->key == user) {
            return opt;
        }
        opt++;
    }
    return NULL;
}


static struct carg_option *
_option_bylongname(struct carg_option *opt, const char *user, int len) {
    if (CMP(user, "help", 4)) {
        return &opt_help;
    }
    else if (CMP(user, "version", 7)) {
        return &opt_version;
    }
    else if (CMP(user, "usage", 5)) {
        return &opt_usage;
    }

    while (opt->longname) {
        if (CMP(user, opt->longname, len)) {
            return opt;
        }
        opt++;
    }
    return NULL;
}


static struct carg_option *
_find_opt(struct carg_state *state, const char **value) {
    const char *user = state->argv[state->current];
    char *tmp;
    int len = strlen(user);
    enum carg_argtype argtype;
    struct carg_option *opt = NULL;

    if (len == 0) {
        return NULL;
    }

    if ((len >= 2) && (user[0] == '-')) {
        user++;
        len--;
        if (user[0] == '-') {
            user++;
            len--;
            argtype = CAT_LONG;
        }
        else {
            argtype = CAT_SHORT;
        }
    }
    else {
        argtype = CAT_COMMAND;
    }

    if (len == 0) {
        return NULL;
    }

    switch (argtype) {
        case CAT_SHORT:
            opt = _option_bykey(state->carg->options, user[0]);
            if (opt && (len > 1)) {
                *value = user + 1;
            }
            break;

        case CAT_LONG:
            tmp = strchr(user, '=');
            if (tmp) {
                *value = tmp + 1;
                tmp[0] = '\0';
            }
            opt = _option_bylongname(state->carg->options, user, len);
            break;

        case CAT_COMMAND:
            // TODO: Implement
            opt = NULL;

        default:
            opt = NULL;
    }

    return opt;
}


static enum carg_eatstatus
_eatopt(int key, const char *value, struct carg_state *state) {
    switch (key) {
        case 'h':
            carg_print_help(state);
            return CARG_EAT_OK_EXIT;

        case '?':
            carg_print_usage(state);
            return CARG_EAT_OK_EXIT;

        case 'V':
            if (state->carg->version == NULL) {
                return CARG_EAT_UNRECOGNIZED;
            }
            dprintf(state->fd, "%s\n", state->carg->version);
            return CARG_EAT_OK_EXIT;
    }

    return CARG_EAT_UNRECOGNIZED;
}


enum carg_status
carg_parse(struct carg *c, int argc, char **argv, void *userptr) {
    int i;
    enum carg_eatstatus eatresult;
    struct carg_option *opt;
    int key;
    const char *value = NULL;

    if (argc < 1) {
        return CARG_ERR;
    }

    struct carg_state state = {
        .carg = c,
        .argc = argc,
        .argv = argv,
        .fd = _outfile,
        .userptr = userptr,
        .current = -1,
        .next = -1,
        .posindex = -1,
    };

    for (i = 1; i < argc; i++) {
        /* Preserve current and next arg */
        state.current = i;
        state.next = (i + 1) >= argc? -1: i + 1;
        value = NULL;

        if (!state.dashdash) {
            /* Find option */
            opt = _find_opt(&state, &value);
            if (opt != NULL) {
                /* Option found */
                if (opt->arg) {
                    /* Option requires argument */
                    if (value == NULL) {
                        if (state.next == -1) {
                            _value_required(&state);
                            return CARG_ERR;
                        }

                        value = argv[i + 1];
                        i++;
                    }
                }
                else if (value) {
                    /* Option not requires any argument */
                    _unrecognized_option(&state);
                    return CARG_ERR;
                }

                key = opt->key;
            }
            else if ((strlen(argv[i]) == 2) && CMP("--", argv[i], 2)) {
                state.dashdash = true;
                continue;
            }
        }

        if (opt == NULL) {
            /* It's not an option, dont startswith: '-' or '--' */
            key = CARG_POSITIONAL;
            value = argv[i];
            state.posindex++;
        }

        /* Try to solve it internaly */
        if (opt != NULL) {
            eatresult = _eatopt(opt->key, value, &state);
            if (eatresult == CARG_EAT_OK_EXIT) {
                return CARG_OK_EXIT;
            }
        }

        /* Raise programming error if eat function is not specified */
        if (state.carg->eat == NULL) {
            _not_eaten(&state, opt);
            return CARG_ERR;
        }

        /* Ask user to solve it */
        eatresult = state.carg->eat(key, value, &state);
        switch (eatresult) {
            case CARG_EAT_OK_EXIT:
                return CARG_OK_EXIT;

            case CARG_EAT_UNRECOGNIZED:
                _not_eaten(&state, opt);
                return CARG_ERR;

            case CARG_EAT_OPT_VALUE:
                _value_required(&state);
                return CARG_ERR;

            case CARG_EAT_ARG_INSUFFICIENT:
                _arg_insufficient(&state);
                return CARG_ERR;
        }
    }

    /* Notify the termination to user */
    /* It's normal to user unrecognize this key, so ignoring */
    eatresult = state.carg->eat(CARG_END, NULL, &state);
    switch (eatresult) {
        case CARG_EAT_OK_EXIT:
            return CARG_OK_EXIT;

        case CARG_EAT_OPT_VALUE:
            _value_required(&state);
            return CARG_ERR;

        case CARG_EAT_ARG_INSUFFICIENT:
            _arg_insufficient(&state);
            return CARG_ERR;
    }
    return CARG_OK;
}
