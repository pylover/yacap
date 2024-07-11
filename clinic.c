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


/*
#define VERBOSITY_DEFAULT  CLOG_WARNING
*/


// enum carg_status
// carg_parse(const struct carg *c, int argc, const char **argv, void *userptr,
//         void **handler) {
//     int i;
//     enum carg_eatstatus eatresult;
//     const struct carg_option *opt;
//     int key;
//     bool next_is_value = false;
//     const char *value = NULL;
//
//     if (argc < 1) {
//         return CARG_ERR;
//     }
//
//     struct carg_state state = {
//         .carg = c,
//         .argc = argc,
//         .argv = argv,
//         .fd = _outfile,
//         .userptr = userptr,
//         .index = -1,
//         .next = NULL,
//         .arg_index = -1,
//     };
//
//     for (i = 1; i < argc; i++) {
//         /* Preserve current index and next arg */
//         state.index = i;
//         if ((i + 1) >= argc) {
//             state.last = true;
//             state.next = NULL;
//         }
//         else {
//             state.last = false;
//             state.next = argv[i + 1];
//         }
//         value = NULL;
//         next_is_value = false;
//
//         if (state.dashdash) {
//             opt = NULL;
//             goto positional;
//         }
//
//         /* Find option */
//         opt = _find_opt(&state, &value);
//         if (opt) {
//             /* Option found */
//             if ((opt->arg) && (value == NULL)) {
//                 /* Option requires argument */
//                 if ((!HASFLAG(opt, CARG_OPTIONAL_VALUE)) &&
//                         state.last) {
//                     _value_required(&state);
//                     return CARG_ERR;
//                 }
//
//                 if (!state.last) {
//                     next_is_value = true;
//                 }
//             }
//             else if ((opt->arg == NULL) && value) {
//                 /* Option not requires any argument */
//                 _unrecognized_option(&state);
//                 return CARG_ERR;
//             }
//
//             /* Preserve key and value */
//             key = opt->key;
//             if (next_is_value && (!state.last)) {
//                 value = state.next;
//             }
//         }
//         else if ((strlen(argv[i]) == 2) && CMP("--", argv[i], 2)) {
//                 state.dashdash = true;
//                 continue;
//         }
//         else {
// positional:
//             value = argv[i];
//             /* It's not an option, it doesn't startswith: '-' or '--' */
//             key = KEY_ARG;
//             state.arg_index++;
//         }
//
//         /* Try to eat argument */
//         eatresult = _eat(key, value, &state);
//         switch (eatresult) {
//             case CARG_EAT_OK:
//                 if (next_is_value) {
//                     /* Next argument is eaten as option's value. */
//                     i++;
//                 }
//                 continue;
//
//             case CARG_EAT_FLAG:
//                 /* Next argument is not eaten by user. just continue */
//                 continue;
//
//             case CARG_EAT_OK_EXIT:
//                 return CARG_OK_EXIT;
//
//             case CARG_EAT_UNRECOGNIZED:
//                 _not_eaten(&state, opt);
//                 return CARG_ERR;
//
//             case CARG_EAT_VALUE_REQUIRED:
//                 _value_required(&state);
//                 return CARG_ERR;
//
//             case CARG_EAT_BAD_VALUE:
//                 _invalid_value(&state, value);
//                 return CARG_ERR;
//
//             case CARG_EAT_ARG_REQUIRED:
//                 _arg_insufficient(&state);
//                 return CARG_ERR;
//         }
//     }
//
//     /* Notify the termination to user */
//     return _notify_finish(&state);
// }


//
//
//
//
// enum carg_argtype {
//     CAT_COMMAND,
//     CAT_SHORT,
//     CAT_LONG,
// };
//
//
// int
// option_count(struct carg_option *options) {
//     if (options == NULL) {
//         return -1;
//     }
//
//     struct carg_option *opt = options;
//     int count = 0;
//
//     while (opt->name) {
//         count++;
//         opt++;
//     }
//
//     return count;
// }
//
//
// static void
// _unrecognized_option(struct carg_state *state) {
//     const char *prog = state->argv[0];
//     dprintf(_errfile, "%s: unrecognized option '%s'\n", prog,
//             state->argv[state->index]);
//     TRYHELP(prog);
// }
//
//
// static void
// _not_eaten(struct carg_state *state, const struct carg_option *opt) {
//     const char *prog = state->argv[0];
//     const char *c = state->argv[state->index];
//
//     if (opt) {
//         dprintf(_errfile, "%s: -%c: (PROGRAM ERROR) "
//                 "Option should have been recognized!?\n", prog, opt->key);
//     }
//     else if (c[0] == '-') {
//         _unrecognized_option(state);
//         return;
//     }
//     else {
//         dprintf(_errfile, "%s: Invalid argument: %s\n", prog, c);
//     }
//     TRYHELP(prog);
// }
//
//
// static void
// _value_required(struct carg_state *state) {
//     const char *prog = state->argv[0];
//     dprintf(_errfile, "%s: '%s' option requires an argument\n", prog,
//             state->argv[state->index]);
//     TRYHELP(prog);
// }
//
//
// static void
// _invalid_value(struct carg_state *state, const char *value) {
//     const char *prog = state->argv[0];
//     dprintf(_errfile, "%s: '%s' option, invalid argument: %s\n", prog,
//             state->argv[state->index], value);
//     TRYHELP(prog);
// }
//
//
// static void
// _arg_insufficient(struct carg_state *state) {
//     const char *prog = state->argv[0];
//     dprintf(_errfile, "%s: insufficient argument(s)\n", prog);
//     TRYHELP(prog);
// }
//
//
// static const struct carg_option *
// _option_bykey(struct carg_state *state, const char user) {
//     const struct carg_option *opt = state->carg->options;
//     const struct carg *c = state->carg;
//
//     switch (user) {
//         case 'h':
//             if (HASFLAG(c, CARG_NO_HELP)) {
//                 goto search;
//             }
//             return &opt_help;
//
//         case 'V':
//             if (c->version == NULL) {
//                 goto search;
//             }
//             return &opt_version;
//
//         case 'v':
//             if (HASFLAG(c, CARG_NO_CLOG)) {
//                 goto search;
//             }
//             return &opt_verbosity;
//
//         case '?':
//             if (HASFLAG(c, CARG_NO_USAGE)) {
//                 goto search;
//             }
//             return &opt_usage;
//     }
//
// search:
//     while (opt->name) {
//         if (opt->key == user) {
//             return opt;
//         }
//         opt++;
//     }
//     return NULL;
// }
//
//
// static const struct carg_option *
// _option_bylongname(struct carg_state *state, const char *user, int len) {
//     const struct carg_option *opt = state->carg->options;
//     const struct carg *c = state->carg;
//
//     if ((!HASFLAG(c, CARG_NO_HELP)) && CMP(user, opt_help.name, len)) {
//         return &opt_help;
//     }
//     else if ((c->version != NULL) && CMP(user, opt_version.name, len)) {
//         return &opt_version;
//     }
//     else if ((!HASFLAG(c, CARG_NO_CLOG)) &&
//             CMP(user, opt_verbosity.name, len)) {
//         return &opt_verbosity;
//     }
//     else if ((!HASFLAG(c, CARG_NO_USAGE)) &&
//             CMP(user, opt_usage.name, len)) {
//         return &opt_usage;
//     }
//
// search:
//     while (opt->name) {
//         if (CMP(user, opt->name, len)) {
//             return opt;
//         }
//         opt++;
//     }
//     return NULL;
// }
//
//
// static const struct carg_option *
// _find_opt(struct carg_state *state, const char **value) {
//     const char *user = state->argv[state->index];
//     char *tmp;
//     int len = strlen(user);
//     enum carg_argtype argtype;
//     const struct carg_option *opt = NULL;
//
//     if (len == 0) {
//         return NULL;
//     }
//
//     if ((len >= 2) && (user[0] == '-')) {
//         user++;
//         len--;
//         if (user[0] == '-') {
//             user++;
//             len--;
//             argtype = CAT_LONG;
//         }
//         else {
//             argtype = CAT_SHORT;
//         }
//     }
//     else {
//         argtype = CAT_COMMAND;
//     }
//
//     if (len == 0) {
//         return NULL;
//     }
//
//     switch (argtype) {
//         case CAT_SHORT:
//             opt = _option_bykey(state, user[0]);
//             if (opt && (len > 1)) {
//                 *value = user + 1;
//             }
//             break;
//
//         case CAT_LONG:
//             tmp = strchr(user, '=');
//             if (tmp) {
//                 *value = tmp + 1;
//                 tmp[0] = '\0';
//             }
//             opt = _option_bylongname(state, user, len);
//             break;
//
//         case CAT_COMMAND:
//             // TODO: Implement
//             opt = NULL;
//
//         default:
//             opt = NULL;
//     }
//
//     return opt;
// }
//
//
// static enum carg_eatstatus
// _eat(int key, const char *value, struct carg_state *state) {
//     int valuelen;
//     const struct carg *c = state->carg;
//
//     /* Try to solve it internaly */
//     switch (key) {
//         case 'h':
//             carg_print_help(state);
//             break;
//
//         case '?':
//             carg_print_usage(state);
//             break;
//
//         case 'V':
//             if (state->carg->version == NULL) {
//                 goto usereat;
//             }
//             dprintf(state->fd, "%s\n", state->carg->version);
//             break;
//
//         case 'v':
//             if (state->carg->flags & CARG_NO_CLOG) {
//                 goto usereat;
//             }
//             valuelen = value? strlen(value): 0;
//
//             if (valuelen == 0) {
//                 clog_verbosity = CLOG_INFO;
//                 return CARG_EAT_FLAG;
//             }
//
//             if (valuelen == 1) {
//                 if (value[0] == 'v') {
//                     /* -vv */
//                     clog_verbosity = CLOG_DEBUG;
//                     return CARG_EAT_OK;
//                 }
//
//                 if (ISDIGIT(value[0])) {
//                     /* -v0 ... -v5 */
//                     clog_verbosity = atoi(value);
//                     if (!BETWEEN(clog_verbosity, CLOG_SILENT, CLOG_DEBUG)) {
//                         clog_verbosity = CLOG_INFO;
//                         return CARG_EAT_BAD_VALUE;
//                     }
//                     return CARG_EAT_OK;
//                 }
//             }
//
//             clog_verbosity = clog_verbosity_from_string(value);
//             if (clog_verbosity == CLOG_UNKNOWN) {
//                 clog_verbosity = CLOG_INFO;
//                 return CARG_EAT_BAD_VALUE;
//             }
//             return CARG_EAT_OK;
//
//         default:
//             goto usereat;
//     }
//     return CARG_EAT_OK_EXIT;
//
// usereat:
//
//     /* Raise programming error if eat function is not specified */
//     if (c->eat == NULL) {
//         return CARG_EAT_UNRECOGNIZED;
//     }
//
//     /* Ask user to solve it */
//     return c->eat(key, value, state);
// }
//
//
// static enum carg_status
// _notify_finish(struct carg_state *state) {
//     /* It's normal to user unrecognize this key, so ignoring */
//     if (state->carg->eat == NULL) {
//         return CARG_OK;
//     }
//
//     switch (state->carg->eat(KEY_END, NULL, state)) {
//         case CARG_EAT_OK_EXIT:
//             return CARG_OK_EXIT;
//
//         case CARG_EAT_VALUE_REQUIRED:
//             _value_required(state);
//             return CARG_ERR;
//
//         case CARG_EAT_ARG_REQUIRED:
//             _arg_insufficient(state);
//             return CARG_ERR;
//     }
//     return CARG_OK;
// }
