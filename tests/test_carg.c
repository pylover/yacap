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
test_version() {
    struct carg carg = {
        .args = NULL,
        .doc = NULL,
        .eat = NULL,
        .options = nooption,
        .footer = NULL,
        .version = "foo 1.2.3",
    };

    char out[1024] = "\0";
    char err[1024] = "\0";

    eqint(CARG_ERR, carg_parse_string(&carg, out, err, "foo -V2"));
    eqstr("", out);
    eqstr("foo: unrecognized option '-V2'\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, out, err, "foo --version"));
    eqstr("foo 1.2.3\n", out);
    eqstr("", err);

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, out, err, "foo -V"));
    eqstr("foo 1.2.3\n", out);
    eqstr("", err);
}


int
main() {
    test_version();
    return EXIT_SUCCESS;
}
