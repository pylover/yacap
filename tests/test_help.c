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


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <clog.h>
#include <cutest.h>

#include "carg.h"
#include "helpers.h"


void
test_help_arg() {
    struct carg carg = {
        .args = "bar\nbaz",
        .doc = NULL,
        .options = nooption,
        .footer = NULL,
        .version = NULL,
    };

    char *help =
        "Usage: foo [OPTION...] bar\n"
        "   or: foo [OPTION...] baz\n"
        "\n"
        "  -h, --help           Give this help list\n"
        "  -?, --usage          Give a short usage message\n"
        "\n";


    char out[1024] = "\0";
    char err[1024] = "\0";
    eqint(1, carg_parse_string(&carg, out, err, "foo --help"));
    eqstr(help, out);
    eqstr("", err);
}


void
test_help_doc() {
    struct carg carg = {
        .args = NULL,
        .doc = LOREM,
        .options = nooption,
        .footer = LOREM,
        .version = NULL,
    };

    char *help =
"Usage: foo [OPTION...]\n"  // NOLINT
"Lorem merol ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod \n"  // NOLINT
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, q-\n"  // NOLINT
"uis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequ-\n"  // NOLINT
"at. Duis aute irure dolor.\n"  // NOLINT
"\n"  // NOLINT
"  -h, --help           Give this help list\n"  // NOLINT
"  -?, --usage          Give a short usage message\n"  // NOLINT
"\n"  // NOLINT
"Lorem merol ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod \n"  // NOLINT
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, q-\n"  // NOLINT
"uis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequ-\n"  // NOLINT
"at. Duis aute irure dolor.\n";  // NOLINT

    char out[1024] = "\0";
    char err[1024] = "\0";
    eqint(1, carg_parse_string(&carg, out, err, "foo --help"));
    eqstr(help, out);
    eqstr("", err);
}


void
test_help_nooptions() {
    struct carg carg = {
        .args = "FOO",
        .doc = NULL,
        .options = nooption,
        .footer = "Lorem ipsum footer",
        .version = "1.0.0a",
    };

    char *help =
        "Usage: foo [OPTION...] FOO\n"
        "\n"
        "  -h, --help           Give this help list\n"
        "  -?, --usage          Give a short usage message\n"
        "  -V, --version        Print program version\n"
        "\n"
        "Lorem ipsum footer\n";


    char out[1024] = "\0";
    char err[1024] = "\0";
    eqint(1, carg_parse_string(&carg, out, err, "foo --help"));
    eqstr(help, out);
    eqstr("", err);
}


void
test_help_options() {
    struct carg_option options[] = {
        {"foo", 'f', NULL, "Foo flag"},
        {"bar", 'b', "BAR", "Bar option with value"},
        {"baz", 'z', "BAZ", LOREM},
        {NULL}
    };

    struct carg carg = {
        .args = NULL,
        .doc = NULL,
        .options = options,
        .footer = "Lorem ipsum footer",
        .version = NULL,
    };

    char *help =
"Usage: foo [OPTION...]\n"
"\n"
"  -f, --foo            Foo flag\n"
"  -b, --bar=BAR        Bar option with value\n"
"  -z, --baz=BAZ        Lorem merol ipsum dolor sit amet, consectetur adipiscing\n"  // NOLINT
"                       elit, sed do eiusmod tempor incididunt ut labore et dol-\n"  // NOLINT
"                       ore magna aliqua. Ut enim ad minim veniam, quis nostrud \n"  // NOLINT
"                       exercitation ullamco laboris nisi ut aliquip ex ea comm-\n"  // NOLINT
"                       odo consequat. Duis aute irure dolor.\n"  // NOLINT
"  -h, --help           Give this help list\n"
"  -?, --usage          Give a short usage message\n"
"\n"
"Lorem ipsum footer\n";


    char out[1024] = "\0";
    char err[1024] = "\0";
    eqint(1, carg_parse_string(&carg, out, err, "foo --help"));
    eqstr(help, out);
    eqstr("", err);
}


int
main() {
    test_help_arg();
    test_help_doc();
    test_help_nooptions();
    test_help_options();
    return EXIT_SUCCESS;
}
