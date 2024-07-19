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


static struct carg carg = {
    .args = NULL,
    .header = NULL,
    .eat = NULL,
    .options = NULL,
    .footer = NULL,
    .version = NULL,
    .flags = 0,
};


static void
test_verbose_short() {
    /* default logging level */
    clog_verbosity = CLOG_UNKNOWN;
    eqint(CARG_OK, carg_parse_string(&carg, "foo", NULL));
    eqint(CLOG_WARNING, clog_verbosity);

    clog_verbosity = CLOG_UNKNOWN;
    eqint(CARG_OK, carg_parse_string(&carg, "foo -v", NULL));
    eqint(CLOG_INFO, clog_verbosity);

    clog_verbosity = CLOG_UNKNOWN;
    eqint(CARG_OK, carg_parse_string(&carg, "foo -vv", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = CLOG_UNKNOWN;
    eqint(CARG_OK, carg_parse_string(&carg, "foo -vvv", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);
}


static void
test_verbose_long() {
    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .eat = NULL,
        .options = NULL,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
    };

    clog_verbosity = -1;
    eqint(CARG_USERERROR, carg_parse_string(&carg, "foo --verbosity", NULL));

    clog_verbosity = -1;
    eqint(CARG_OK, carg_parse_string(&carg, "foo --verbosity d", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(CARG_OK, carg_parse_string(&carg, "foo --verbosity debug", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(CARG_OK, carg_parse_string(&carg, "foo --verbosity=debug", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(CARG_OK, carg_parse_string(&carg, "foo --verbosity 2", NULL));
    eqint(CLOG_ERROR, clog_verbosity);
}


int
main() {
    test_verbose_short();
    test_verbose_long();
    return EXIT_SUCCESS;
}
