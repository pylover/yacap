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
#ifndef OPTION_H_
#define OPTION_H_


#include <stdbool.h>


/* option flags */
enum carg_optionflags {
    CARG_OPTION = 0,
    CARG_OPTIONAL_VALUE = 1,
    CARG_COMMAND = 2,
};


struct carg_option {
    const char *name;
    const int key;
    const char *arg;
    enum carg_optionflags flags;
    const char *help;
};


struct carg_option *
option_findbykey(struct carg_option *options, int key);


#endif  // OPTION_H_

