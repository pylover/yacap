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

#include "yacap.h"
#include "helpers.h"


static void
test_version() {
    struct yacap yacap = {
        .args = NULL,
        .header = NULL,
        .eat = NULL,
        .options = NULL,
        .footer = NULL,
        .version = "foo 1.2.3",
        .flags = 0,
    };

    eqint(YACAP_OK_EXIT, yacap_parse_string(&yacap, "foo --version", NULL));
    eqstr("foo 1.2.3\n", out);
    eqstr("", err);
}


int
main() {
    test_version();
    return EXIT_SUCCESS;
}
