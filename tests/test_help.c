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
test_usage() {
    struct carg carg = {
        .args = "bar\nbaz",
        .header = NULL,
        .options = NULL,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
    };

    const char *usage =
        "Usage: foo [OPTION...] bar\n"
        "   or: foo [OPTION...] baz\n";

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --usage", NULL));
    eqstr(usage, out);
    eqstr("", err);

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo -?", NULL));
    eqstr(usage, out);
    eqstr("", err);
}


void
test_help_doc() {
    struct carg carg = {
        .args = NULL,
        .header = LOREM,
        .options = NULL,
        .footer = LOREM,
        .version = NULL,
    };

#ifdef CARG_USE_CLOG

    char *help =
"Usage: foo [OPTION...]\n"
"\n"
"Lorem merol ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod\n"  // NOLINT
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, q-\n"  // NOLINT
"uis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequ-\n"  // NOLINT
"at. Duis aute irure dolor.\n"
"\n"
"  -h, --help               Give this help list and exit\n"
"  -?, --usage              Give a short usage message and exit\n"
"  -v                       Increase the clog_verbosity on each occurance, e.g.\n"  // NOLINT
"                           -vvv\n"
"      --verbosity=LEVEL    Verbosity level. one of: '0|s|silent', '1|f|fatal',\n"  // NOLINT
"                           '2|e|error', '3|w|warn', '4|i|info' and '5|d|debug'.\n"  // NOLINT
"                           if this option is not given, the verbosity level wi-\n"  // NOLINT
"                           ll be '3|w|warn'\n"
"\n"
"Lorem merol ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod\n"  // NOLINT
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, q-\n"  // NOLINT
"uis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequ-\n"  // NOLINT
"at. Duis aute irure dolor.\n";

#else

    char *help =
"Usage: foo [OPTION...]\n"
"\n"
"Lorem merol ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod\n"  // NOLINT
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, q-\n"  // NOLINT
"uis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequ-\n"  // NOLINT
"at. Duis aute irure dolor.\n"
"\n"
"  -h, --help     Give this help list and exit\n"
"  -?, --usage    Give a short usage message and exit\n"
"\n"
"Lorem merol ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod\n"  // NOLINT
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, q-\n"  // NOLINT
"uis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequ-\n"  // NOLINT
"at. Duis aute irure dolor.\n";

#endif

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --help", NULL));
    eqstr(help, out);
    eqstr("", err);
}


void
test_help_nooptions() {
    char *help;
    struct carg carg = {
        .args = "FOO",
        .header = NULL,
        .options = NULL,
        .footer = "Lorem ipsum footer",
        .version = NULL,
        .flags = CARG_NO_HELP
    };

    eqint(CARG_USERERROR, carg_parse_string(&carg, "foo --help", NULL));
    eqstr("", out);
    eqstr("foo: invalid option -- '--help'\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);

    carg.flags = CARG_NO_CLOG | CARG_NO_USAGE;
    help =
        "Usage: foo [OPTION...] FOO\n"
        "\n"
        "  -h, --help    Give this help list and exit\n"
        "\n"
        "Lorem ipsum footer\n";

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --help", NULL));
    eqstr(help, out);
    eqstr("", err);

    carg.flags = CARG_NO_CLOG;
    help =
        "Usage: foo [OPTION...] FOO\n"
        "\n"
        "  -h, --help     Give this help list and exit\n"
        "  -?, --usage    Give a short usage message and exit\n"
        "\n"
        "Lorem ipsum footer\n";

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --help", NULL));
    eqstr(help, out);
    eqstr("", err);
}


void
test_help_default() {
    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .options = NULL,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
    };

#ifdef CARG_USE_CLOG
    char *help =
"Usage: foo [OPTION...]\n"
"\n"
"  -h, --help               Give this help list and exit\n"
"  -?, --usage              Give a short usage message and exit\n"
"  -v                       Increase the clog_verbosity on each occurance, e.g.\n"  // NOLINT
"                           -vvv\n"
"      --verbosity=LEVEL    Verbosity level. one of: '0|s|silent', '1|f|fatal',\n"  // NOLINT
"                           '2|e|error', '3|w|warn', '4|i|info' and '5|d|debug'.\n"  // NOLINT
"                           if this option is not given, the verbosity level wi-\n"  // NOLINT
"                           ll be '3|w|warn'\n";
#else
    char *help =
"Usage: foo [OPTION...]\n"
"\n"
"  -h, --help     Give this help list and exit\n"
"  -?, --usage    Give a short usage message and exit\n";

#endif

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

#ifdef CARG_USE_CLOG
    char *help =
"Usage: foo [OPTION...]\n"
"\n"
"  -f, --foo                Foo flag\n"
"  -b, --bar=BAR            Bar option with value\n"
"  -z, --baz=BAZ            Lorem merol ipsum dolor sit amet, consectetur adipi-\n"  // NOLINT
"                           scing elit, sed do eiusmod tempor incididunt ut lab-\n"  // NOLINT
"                           ore et dolore magna aliqua. Ut enim ad minim veniam,\n"  // NOLINT
"                           quis nostrud exercitation ullamco laboris nisi ut a-\n"  // NOLINT
"                           liquip ex ea commodo consequat. Duis aute irure dol-\n"  // NOLINT
"                           or.\n"
"      --qux=QUX            \n"
"  -h, --help               Give this help list and exit\n"
"  -?, --usage              Give a short usage message and exit\n"
"  -v                       Increase the clog_verbosity on each occurance, e.g.\n"  // NOLINT
"                           -vvv\n"
"      --verbosity=LEVEL    Verbosity level. one of: '0|s|silent', '1|f|fatal',\n"  // NOLINT
"                           '2|e|error', '3|w|warn', '4|i|info' and '5|d|debug'.\n"  // NOLINT
"                           if this option is not given, the verbosity level wi-\n"  // NOLINT
"                           ll be '3|w|warn'\n"
"\n"
"Lorem ipsum footer\n";
#else
    char *help =
"Usage: foo [OPTION...]\n"
"\n"
"  -f, --foo        Foo flag\n"
"  -b, --bar=BAR    Bar option with value\n"
"  -z, --baz=BAZ    Lorem merol ipsum dolor sit amet, consectetur adipiscing el-\n"  // NOLINT
"                   it, sed do eiusmod tempor incididunt ut labore et dolore ma-\n"  // NOLINT
"                   gna aliqua. Ut enim ad minim veniam, quis nostrud exercitat-\n"  // NOLINT
"                   ion ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"  // NOLINT
"                   Duis aute irure dolor.\n"
"      --qux=QUX    \n"
"  -h, --help       Give this help list and exit\n"
"  -?, --usage      Give a short usage message and exit\n"
"\n"
"Lorem ipsum footer\n";
#endif

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
