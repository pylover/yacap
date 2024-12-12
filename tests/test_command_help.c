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
#include <cutest.h>

#include "config.h"
#include "yacap.h"
#include "pipewrap.h"


void
test_command_help() {
    struct yacap_option thud_options[] = {
        {"baz", 'z', NULL, 0, "Baz flag"},
        {NULL}
    };

    const struct yacap_command thud_cmd = {
        .name = "thud",
        .args = "qux",
        .options = thud_options,
        .header = "Header: Lorem ipsum footer",
        .footer = "Footer: Lorem ipsum footer",
        .eat = NULL,
        .userptr = NULL,
    };

    struct yacap_option root_options[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };

    struct yacap yacap = {
        .args = NULL,
        .header = NULL,
        .eat = NULL,
        .options = root_options,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
        .userptr = NULL,
        .commands = (const struct yacap_command*[]) {
            &thud_cmd,
            NULL
        },
    };

    char *help =
#ifdef YACAP_USE_CLOG
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
"  -q                       Decrease the clog_verbosity on each occurance, e.g.\n" // NOLINT
"                           -qq\n"
"      --verbosity=LEVEL    Verbosity level. one of: '0|s|silent', '1|f|fatal',\n"  // NOLINT
"                           '2|e|error', '3|w|warn', '4|i|info' and '5|d|debug'.\n"  // NOLINT
"                           if this option is not given, the verbosity level wi-\n"  // NOLINT
"                           ll be '4|i|info'\n"
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

    eqint(YACAP_OK_EXIT, pipewrap(&yacap, "foo --help", NULL));
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

    eqint(YACAP_OK_EXIT, pipewrap(&yacap, "foo thud --help", NULL));
    eqstr(help, out);
    eqstr("", err);

    help = "Usage: foo thud [OPTION...] qux\n";
    eqint(YACAP_OK_EXIT, pipewrap(&yacap, "foo thud --usage", NULL));
    eqstr(help, out);
    eqstr("", err);
}


int
main() {
    test_command_help();
    return EXIT_SUCCESS;
}
