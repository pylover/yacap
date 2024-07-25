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
#include <clog.h>
#include <cutest.h>

#include "config.h"
#include "carg.h"
#include "helpers.h"


void
test_command_help() {
    struct carg_option thud_options[] = {
        {"baz", 'z', NULL, 0, "Baz flag"},
        {NULL}
    };

    const struct carg_command thud_cmd = {
        .name = "thud",
        .args = "qux",
        .options = thud_options,
        .header = "Header: Lorem ipsum footer",
        .footer = "Footer: Lorem ipsum footer",
        .eat = NULL,
        .userptr = NULL,
    };

    struct carg_option root_options[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };

    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .eat = NULL,
        .options = root_options,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
        .userptr = NULL,
        .commands = (const struct carg_command*[]) {
            &thud_cmd,
            NULL
        },
    };

    char *help =
#ifdef CARG_USE_CLOG
"Usage: foo [OPTION...]\n"
"\n"
"Commands:\n"
"  thud\n"
"\n"
"Options:\n"
"  -h, --help               Give this help list and exit\n"
"  -?, --usage              Give a short usage message and exit\n"
"  -v                       Increase the clog_verbosity on each occurance, e.g.\n"  // NOLINT
"                           -vvv\n"
"      --verbosity=LEVEL    Verbosity level. one of: '0|s|silent', '1|f|fatal',\n"  // NOLINT
"                           '2|e|error', '3|w|warn', '4|i|info' and '5|d|debug'.\n"  // NOLINT
"                           if this option is not given, the verbosity level wi-\n"  // NOLINT
"                           ll be '3|w|warn'\n"
"  -f, --foo                Foo flag\n"
"  -b, --bar=BAR            Bar option with value\n";
#else
"Usage: foo [OPTION...]\n"
"\n"
"Commands:\n"
"  thud\n"
"\n"
"Options:\n"
"  -h, --help       Give this help list and exit\n"
"  -?, --usage      Give a short usage message and exit\n"
"  -f, --foo        Foo flag\n"
"  -b, --bar=BAR    Bar option with value\n";
#endif

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --help", NULL));
    eqstr(help, out);
    eqstr("", err);

    help =
"Usage: foo thud [OPTION...] qux\n"
"\n"
"Header: Lorem ipsum footer\n"
"\n"
"Options:\n"
"  -h, --help     Give this help list and exit\n"
"  -?, --usage    Give a short usage message and exit\n"
"  -z, --baz      Baz flag\n"
"\n"
"Footer: Lorem ipsum footer\n";

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo thud --help", NULL));
    eqstr(help, out);
    eqstr("", err);

    help = "Usage: foo thud [OPTION...] qux\n";
    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo thud --usage", NULL));
    eqstr(help, out);
    eqstr("", err);
}


int
main() {
    test_command_help();
    return EXIT_SUCCESS;
}
