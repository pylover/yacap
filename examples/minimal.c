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
#include <stdio.h>
#include <stdlib.h>

#include "carg.h"


static enum carg_eatstatus
_eat(const struct carg_option *opt, const char *value) {
    printf("%s\n", value);
    return CARG_EAT_OK;
}


/* create and configure a CArg structure */
static struct carg cli = {
    .eat = (carg_eater_t)_eat,
};


int
main(int argc, const char **argv) {
    enum carg_status status = carg_parse(&cli, argc, argv, NULL);
    carg_dispose(&cli);

    if (status >= CARG_OK) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
