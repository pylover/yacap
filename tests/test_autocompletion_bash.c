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
#include "suggest.h"


static struct yacap_command bar_cmd = {
    .name = "bar",
};


static struct yacap root = {
    .name = "foo",
    .commands = (const struct yacap_command*[]) {
        &bar_cmd,
        NULL
    },
};


void
test_autocompletion_bash() {
    eqint(0, suggest(&root, "foo "));
    // eqstr("bar", suggestions[0]);
    // isnull(suggestions[1]);
}


int
main() {
    test_autocompletion_bash();
    return EXIT_SUCCESS;
}
