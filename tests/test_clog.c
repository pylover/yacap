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
#include <clog.h>
#include <cutest.h>

#include "yacap.h"
#include "helpers.h"


static struct yacap yacap = {
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
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo", NULL));
    eqint(CLOG_INFO, clog_verbosity);

    clog_verbosity = CLOG_UNKNOWN;
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo -v", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = CLOG_UNKNOWN;
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo -vv", NULL));
    eqint(CLOG_DEBUG2, clog_verbosity);

    clog_verbosity = CLOG_UNKNOWN;
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo -vvv", NULL));
    eqint(CLOG_DEBUG2, clog_verbosity);
}


static void
test_verbose_long() {
    struct yacap yacap = {
        .args = NULL,
        .header = NULL,
        .eat = NULL,
        .options = NULL,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
    };

    clog_verbosity = -1;
    eqint(YACAP_USERERROR, yacap_parse_string(&yacap, "foo --verbosity", NULL));

    clog_verbosity = -1;
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo --verbosity d", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo --verbosity debug", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo --verbosity=debug", NULL));
    eqint(CLOG_DEBUG, clog_verbosity);

    clog_verbosity = -1;
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo --verbosity 2", NULL));
    eqint(CLOG_ERROR, clog_verbosity);
}


int
main() {
    test_verbose_short();
    test_verbose_long();
    return EXIT_SUCCESS;
}
