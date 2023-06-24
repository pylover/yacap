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


#include <clog.h>
#include <cutest.h>

#include "carg.h"
#include "helpers.h"


void
test_usage() {
    struct carg carg = {
        .args = "bar\nbaz",
        .header = NULL,
        .options = nooption,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
    };

    char *usage =
        "Usage: foo [OPTION...] bar\n"
        "   or: foo [OPTION...] baz\n";

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --usage", NULL));
    eqstr(usage, out);
    eqstr("", err);
}


void
test_help_doc() {
    struct carg carg = {
        .args = NULL,
        .header = LOREM,
        .options = nooption,
        .footer = LOREM,
        .version = NULL,
    };

    char *help =
"Usage: foo [OPTION...]\n"  // NOLINT
"\n"  // NOLINT
"Lorem merol ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod \n"  // NOLINT
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, q-\n"  // NOLINT
"uis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequ-\n"  // NOLINT
"at. Duis aute irure dolor.\n"  // NOLINT
"\n"  // NOLINT
"  -h, --help               Give this help list\n"  // NOLINT
"  -?, --usage              Give a short usage message\n"  // NOLINT
"  -v, --verbose[=LEVEL]    Verbosity level. one of: 0|s|silent, 1|f|fatal, 2|e|\n"  // NOLINT
"                           error, 3|w|warn, 4|i|info 5|d|debug. default: warn.\n"  // NOLINT
"\n"  // NOLINT
"Lorem merol ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod \n"  // NOLINT
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, q-\n"  // NOLINT
"uis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequ-\n"  // NOLINT
"at. Duis aute irure dolor.\n";  // NOLINT

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --help", NULL));
    eqstr(help, out);
    eqstr("", err);
}


void
test_help_nooptions() {
    struct carg carg = {
        .args = "FOO",
        .header = NULL,
        .options = nooption,
        .footer = "Lorem ipsum footer",
        .version = NULL,
        .flags = NO_HELP
    };

    eqint(CARG_ERR, carg_parse_string(&carg, "foo --help", NULL));
    eqstr("", out);
    eqstr("foo: unrecognized option '--help'\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);

    carg.flags = NO_CLOG | NO_USAGE;
    char *help =
        "Usage: foo [OPTION...] FOO\n"
        "\n"
        "  -h, --help    Give this help list\n"
        "\n"
        "Lorem ipsum footer\n";

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --help", NULL));
    eqstr(help, out);
    eqstr("", err);
}


void
test_help_default() {
    struct carg carg = {
        .args = "FOO",
        .header = NULL,
        .options = nooption,
        .footer = "Lorem ipsum footer",
        .version = "1.0.0a",
        .flags = 0,
    };

    char *help =
        "Usage: foo [OPTION...] FOO\n"
        "\n"
        "  -h, --help               Give this help list\n"  // NOLINT
        "  -?, --usage              Give a short usage message\n"  // NOLINT
        "  -v, --verbose[=LEVEL]    Verbosity level. one of: 0|s|silent, 1|f|fatal, 2|e|\n"  // NOLINT
        "                           error, 3|w|warn, 4|i|info 5|d|debug. default: warn.\n"  // NOLINT
        "  -V, --version            Print program version\n"  // NOLINT
        "\n"
        "Lorem ipsum footer\n";

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --help", NULL));
    eqstr(help, out);
    eqstr("", err);
}


void
test_help_options() {
    struct carg_option options[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {"baz", 'z', "BAZ", 0, LOREM},
        {"qux", 1, "QUX",  0, NULL},
        {NULL}
    };

    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .options = options,
        .footer = "Lorem ipsum footer",
        .version = NULL,
    };

    char *help =
"Usage: foo [OPTION...]\n"
"\n"
"  -f, --foo                Foo flag\n"  // NOLINT
"  -b, --bar=BAR            Bar option with value\n"  // NOLINT
"  -z, --baz=BAZ            Lorem merol ipsum dolor sit amet, consectetur adipi-\n"  // NOLINT
"                           scing elit, sed do eiusmod tempor incididunt ut lab-\n"  // NOLINT
"                           ore et dolore magna aliqua. Ut enim ad minim veniam,\n"  // NOLINT
"                           quis nostrud exercitation ullamco laboris nisi ut a-\n"  // NOLINT
"                           liquip ex ea commodo consequat. Duis aute irure dol-\n"  // NOLINT
"                           or.\n"  // NOLINT
"      --qux=QUX            \n"  // NOLINT
"  -h, --help               Give this help list\n"  // NOLINT
"  -?, --usage              Give a short usage message\n"  // NOLINT
"  -v, --verbose[=LEVEL]    Verbosity level. one of: 0|s|silent, 1|f|fatal, 2|e|\n"  // NOLINT
"                           error, 3|w|warn, 4|i|info 5|d|debug. default: warn.\n"  // NOLINT
"\n"
"Lorem ipsum footer\n";

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --help", NULL));
    eqstr(help, out);
    eqstr("", err);
}


int
main() {
    test_usage();
    test_help_doc();
    test_help_default();
    test_help_nooptions();
    test_help_options();
    return EXIT_SUCCESS;
}
