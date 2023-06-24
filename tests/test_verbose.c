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


static struct carg carg = {
    .args = NULL,
    .header = NULL,
    .eat = NULL,
    .options = nooption,
    .footer = NULL,
    .version = "foo 1.2.3",
    .flags = 0,
};


static void
test_verbose_short() {
    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo -v", NULL));
    eqint(CLOG_INFO, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo -vv", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo -v2", NULL));
    eqint(CLOG_ERROR, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo -v0", NULL));
    eqint(CLOG_SILENT, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo -ve", NULL));
    eqint(CLOG_ERROR, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo -vw", NULL));
    eqint(CLOG_WARNING, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo -vi", NULL));
    eqint(CLOG_INFO, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo -vd", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_ERR, carg_parse_string(&carg, "foo -vx", NULL));
    eqstr("", out);
    eqstr("foo: '-vx' option, invalid argument: x\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);

    clog_verbosity = -1;
    eqint(STATUS_ERR, carg_parse_string(&carg, "foo -v6", NULL));
    eqstr("", out);
    eqstr("foo: '-v6' option, invalid argument: 6\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);

    clog_verbosity = -1;
    eqint(STATUS_ERR, carg_parse_string(&carg, "foo -v-1", NULL));
    eqstr("", out);
    eqstr("foo: '-v-1' option, invalid argument: -1\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);

    clog_verbosity = -1;
    eqint(STATUS_ERR, carg_parse_string(&carg, "foo -vvv", NULL));
    eqstr("", out);
    eqstr("foo: '-vvv' option, invalid argument: vv\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);
}


static void
test_verbose_long() {
    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .eat = NULL,
        .options = nooption,
        .footer = NULL,
        .version = "foo 1.2.3",
        .flags = 0,
    };

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo --verbose", NULL));
    eqint(CLOG_INFO, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo --verbose d", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo --verbose debug", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo --verbose=debug", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(STATUS_OK, carg_parse_string(&carg, "foo --verbose 2", NULL));
    eqint(CLOG_ERROR, clog_verbosity);
}


int
main() {
    test_verbose_short();
    test_verbose_long();
    return EXIT_SUCCESS;
}
