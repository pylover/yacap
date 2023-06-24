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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <clog.h>

#include "carg.h"


#define VERBOSITY_DEFAULT  CLOG_WARNING
#define HELP_LINESIZE 79
#define USAGE_BUFFSIZE 1024
#define TRYHELP(p) dprintf(_errfile, \
        "Try `%s --help' or `%s --usage' for more information.\n", p, p);
#define CMP(x, y, l) (strncmp(x, y, l) == 0)
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
    strlen((o)->longname) + \
    ((o)->arg? strlen((o)->arg) + 1: 0) + \
    (HASFLAG(o, CARG_OPTIONAL_VALUE)? 2: 0))


static int _outfile = STDOUT_FILENO;
static int _errfile = STDERR_FILENO;
static struct carg_option opt_verbosity = {
    .longname = "verbose",
    .key = 'v',
    .arg = "LEVEL",
    .flags = CARG_OPTIONAL_VALUE,
    .help = "Verbosity level. one of: 0|s|silent, 1|f|fatal, 2|e|error, "
        "3|w|warn, 4|i|info 5|d|debug. default: warn."
};
static struct carg_option opt_version = {"version", 'V', NULL, 0,
    "Print program version"};
static struct carg_option opt_help = {"help", 'h', NULL, 0,
    "Give this help list"};
static struct carg_option opt_usage = {"usage", '?', NULL, 0,
    "Give a short usage message"};


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
        else if ((string[ls - 1] != ' ') && (string[ls] != ' ') &&
                (!ISSIGN(string[ls - 1])) && (!ISSIGN(string[ls]))) {
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
_print_option(int fd, struct carg_option *opt, int gapsize) {
    int rpad = gapsize - OPT_HELPLEN(opt);

    if (ISCHAR(opt->key)) {
        dprintf(fd, "  -%c, ", opt->key);
    }
    else {
        dprintf(fd, "      ");
    }

    if (opt->arg == NULL) {
        dprintf(fd, "--%s%*s", opt->longname, rpad, "");
    }
    else if (HASFLAG(opt, CARG_OPTIONAL_VALUE)) {
        dprintf(fd, "--%s[=%s]%*s", opt->longname, opt->arg, rpad, "");
    }
    else {
        dprintf(fd, "--%s=%s%*s", opt->longname, opt->arg, rpad, "");
    }

    if (opt->help) {
        _print_multiline(fd, opt->help, gapsize + 8, HELP_LINESIZE);
    }
    else {
        dprintf(fd, "\n");
    }
}


static int
_calculate_initial_gapsize(struct carg *c) {
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
_print_options(int fd, struct carg *c) {
    int gapsize = _calculate_initial_gapsize(c);
    int i = 0;
    struct carg_option *opt;

    while (true) {
        opt = &(c->options[i++]);
        if (opt->longname == NULL) {
            break;
        }

        gapsize = MAX(gapsize, OPT_HELPLEN(opt) + OPT_MINGAP);
    }

    dprintf(fd, "\n");
    i = 0;
    while (true) {
        opt = &(c->options[i++]);
        if (opt->longname == NULL) {
            break;
        }

        _print_option(fd, opt, gapsize);
    }

    if (!(c->flags & CARG_NO_HELP)) {
        _print_option(fd, &opt_help, gapsize);
    }

    if (!(c->flags & CARG_NO_USAGE)) {
        _print_option(fd, &opt_usage, gapsize);
    }

    if (!(c->flags & CARG_NO_CLOG)) {
        _print_option(fd, &opt_verbosity, gapsize);
    }

    if (c->version) {
        _print_option(fd, &opt_version, gapsize);
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
    if (state->carg->header) {
        dprintf(state->fd, "\n");
        _print_multiline(state->fd, state->carg->header, 0, HELP_LINESIZE);
    }

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
_value_invalid(struct carg_state *state, const char *value) {
    char *prog = state->argv[0];
    dprintf(_errfile, "%s: '%s' option, invalid argument: %s\n", prog,
            state->argv[state->current], value);
    TRYHELP(prog);
}


static void
_arg_insufficient(struct carg_state *state) {
    char *prog = state->argv[0];
    dprintf(_errfile, "%s: insufficient argument(s)\n", prog);
    TRYHELP(prog);
}


static struct carg_option *
_option_bykey(struct carg_state *state, const char user) {
    struct carg_option *opt = state->carg->options;
    struct carg *c = state->carg;

    switch (user) {
        case 'h':
            if (HASFLAG(c, CARG_NO_HELP)) {
                goto search;
            }
            return &opt_help;

        case 'V':
            if (c->version == NULL) {
                goto search;
            }
            return &opt_version;

        case 'v':
            if (HASFLAG(c, CARG_NO_CLOG)) {
                goto search;
            }
            return &opt_verbosity;

        case '?':
            if (HASFLAG(c, CARG_NO_USAGE)) {
                goto search;
            }
            return &opt_usage;
    }

search:
    while (opt->longname) {
        if (opt->key == user) {
            return opt;
        }
        opt++;
    }
    return NULL;
}


static struct carg_option *
_option_bylongname(struct carg_state *state, const char *user, int len) {
    struct carg_option *opt = state->carg->options;
    struct carg *c = state->carg;

    if ((!HASFLAG(c, CARG_NO_HELP)) && CMP(user, opt_help.longname, len)) {
        return &opt_help;
    }
    else if ((c->version != NULL) && CMP(user, opt_version.longname, len)) {
        return &opt_version;
    }
    else if ((!HASFLAG(c, CARG_NO_CLOG)) &&
            CMP(user, opt_verbosity.longname, len)) {
        return &opt_verbosity;
    }
    else if ((!HASFLAG(c, CARG_NO_USAGE)) &&
            CMP(user, opt_usage.longname, len)) {
        return &opt_usage;
    }

search:
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
            opt = _option_bykey(state, user[0]);
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
            opt = _option_bylongname(state, user, len);
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
_eat(int key, const char *value, struct carg_state *state) {
    int valuelen;

    switch (key) {
        case 'h':
            carg_print_help(state);
            break;

        case '?':
            carg_print_usage(state);
            break;

        case 'V':
            if (state->carg->version == NULL) {
                goto ignore;
            }
            dprintf(state->fd, "%s\n", state->carg->version);
            break;

        case 'v':
            if (state->carg->flags & CARG_NO_CLOG) {
                goto ignore;
            }
            valuelen = value? strlen(value): 0;

            if (valuelen == 0) {
                clog_verbosity = CLOG_INFO;
                return CARG_EAT_OPT_WITHOUT_VALUE;
            }

            if (valuelen == 1) {
                if (value[0] == 'v') {
                    /* -vv */
                    clog_verbosity = CLOG_DEBUG;
                    return CARG_EAT_OK;
                }

                if (ISDIGIT(value[0])) {
                    /* -v0 ... -v5 */
                    clog_verbosity = atoi(value);
                    if (!BETWEEN(clog_verbosity, CLOG_SILENT, CLOG_DEBUG)) {
                        clog_verbosity = CLOG_INFO;
                        return CARG_EAT_OPT_BADVALUE;
                    }
                    return CARG_EAT_OK;
                }
            }

            clog_verbosity = clog_verbosity_from_string(value);
            if (clog_verbosity == CLOG_UNKNOWN) {
                clog_verbosity = CLOG_INFO;
                return CARG_EAT_OPT_BADVALUE;
            }
            return CARG_EAT_OK;

        default:
            goto ignore;
    }
    return CARG_EAT_OK_EXIT;

ignore:
    return CARG_EAT_UNRECOGNIZED;
}


enum carg_status
carg_parse(struct carg *c, int argc, char **argv, void *userptr) {
    int i;
    enum carg_eatstatus eatresult;
    struct carg_option *opt;
    int key;
    bool nextasvalue = false;
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
        if ((i + 1) >= argc) {
            state.last = true;
            state.next = -1;
        }
        else {
            state.last = false;
            state.next = i + 1;
        }
        value = NULL;
        nextasvalue = false;

        if (!state.dashdash) {
            /* Find option */
            opt = _find_opt(&state, &value);
            if (opt != NULL) {
                /* Option found */
                if (opt->arg) {
                    /* Option requires argument */
                    if (value == NULL) {
                        if ((!HASFLAG(opt, CARG_OPTIONAL_VALUE)) &&
                                state.last) {
                            _value_required(&state);
                            return CARG_ERR;
                        }

                        if (!state.last) {
                            nextasvalue = true;
                        }
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
            /* It's not an option, it doesn't startswith: '-' or '--' */
            key = CARG_POSITIONAL;
            value = argv[i];
            state.posindex++;
        }
        else if (nextasvalue && (!state.last)) {
            value = argv[state.next];
        }

        /* Try to solve it internaly */
        if (opt != NULL) {
            eatresult = _eat(opt->key, value, &state);
            switch (eatresult) {
                case CARG_EAT_OK:
                    if (nextasvalue) {
                        /* Skip one argument due the next one is eaten as
                           option's value. */
                        i++;
                    }
                    continue;

                case CARG_EAT_OPT_WITHOUT_VALUE:
                    continue;

                case CARG_EAT_OK_EXIT:
                    return CARG_OK_EXIT;

                case CARG_EAT_OPT_BADVALUE:
                    _value_invalid(&state, value);
                    return CARG_ERR;
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
            case CARG_EAT_OK:
                if (nextasvalue) {
                    /* Skip one argument due the next one is eaten as
                       option's value. */
                    i++;
                }
                continue;
            case CARG_EAT_OK_EXIT:
                return CARG_OK_EXIT;

            case CARG_EAT_UNRECOGNIZED:
                _not_eaten(&state, opt);
                return CARG_ERR;

            case CARG_EAT_OPT_VALUE_REQUIRED:
                _value_required(&state);
                return CARG_ERR;

            case CARG_EAT_OPT_BADVALUE:
                _value_invalid(&state, value);
                return CARG_ERR;

            case CARG_EAT_ARG_INSUFFICIENT:
                _arg_insufficient(&state);
                return CARG_ERR;
        }
    }

    /* Notify the termination to user */
    /* It's normal to user unrecognize this key, so ignoring */
    if (state.carg->eat == NULL) {
        return CARG_OK;
    }
    eatresult = state.carg->eat(CARG_END, NULL, &state);
    switch (eatresult) {
        case CARG_EAT_OK_EXIT:
            return CARG_OK_EXIT;

        case CARG_EAT_OPT_VALUE_REQUIRED:
            _value_required(&state);
            return CARG_ERR;

        case CARG_EAT_ARG_INSUFFICIENT:
            _arg_insufficient(&state);
            return CARG_ERR;
    }
    return CARG_OK;
}
