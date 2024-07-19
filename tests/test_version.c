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

#include "carg.h"
#include "helpers.h"


static void
test_version() {
    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .eat = NULL,
        .options = NULL,
        .footer = NULL,
        .version = "foo 1.2.3",
        .flags = 0,
    };

    eqint(CARG_OK_EXIT, carg_parse_string(&carg, "foo --version"));
    eqstr("foo 1.2.3\n", out);
    eqstr("", err);
}


int
main() {
    test_version();
    return EXIT_SUCCESS;
}
