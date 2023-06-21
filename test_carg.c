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

#include <cutest.h>
#include <clog.h>
#include "carg.h"


#define BUFFSIZE    1023
#define LOREM "Lorem merol ipsum dolor sit amet, consectetur adipiscing " \
    "elit, sed do eiusmod tempor incididunt ut labore et dolore magna " \
    "aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco " \
    "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor."


static struct carg_option nooption[] = {{ NULL }};


int
carg_parse_string(struct carg *c, char *out, char *err, const char * line) {
    char *argv[256];
    int argc = 0;
    char delim[1] = {' '};
    char *needle;
    char *saveptr = NULL;
    static char buff[BUFFSIZE + 1];
    strcpy(buff, line);

    needle = strtok_r(buff, delim, &saveptr);
    argv[argc++] = needle;
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        argv[argc++] = needle;
    }

    /* Piping */
    int outpipe[2];
    int errpipe[2];
    out[0] = '\0';
    err[0] = '\0';
    pipe(outpipe);
    pipe(errpipe);

    carg_outfile_set(outpipe[1]);
    carg_errfile_set(errpipe[1]);

    int ret = carg_parse(c, argc, argv);

    close(outpipe[1]);
    close(errpipe[1]);
    read(outpipe[0], out, BUFFSIZE);
    read(errpipe[0], err, BUFFSIZE);
    close(outpipe[0]);
    close(errpipe[0]);

    return ret;
}


/*
Usage: wepn [OPTION...] serve
  or:  wepn [OPTION...] connect <HOST>
  or:  wepn [OPTION...] token list
  or:  wepn [OPTION...] token create
  or:  wepn [OPTION...] token delete <INDEX>

Simple Virtual Private Network

  -c, --configuration-file=FILENAME
                             Configuration filename, default:
                             /etc/wepn/wepn.conf (if exists)
  -v, --verbosity=LEVEL      Verbosity level: 0-5. default: 4
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.
*/
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
