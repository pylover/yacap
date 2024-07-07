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
#ifndef OPTIONDB_H_
#define OPTIONDB_H_


#include "carg.h"


struct carg_optiondb {
    struct carg_optioninfo *repo;
    size_t size;
    volatile size_t count;
};


struct carg_optioninfo {
    const struct carg_option *option;
    int flags;
};


int
optiondb_init(struct carg_optiondb *db);


void
optiondb_dispose(struct carg_optiondb *db);


int
optiondb_insert(struct carg_optiondb *db, const struct carg_option *options);


int
optiondb_exists(struct carg_optiondb *db, const struct carg_option *opt);


const struct carg_optioninfo *
optiondb_findbyname(const struct carg_optiondb *db, const char *name,
        int len);


const struct carg_optioninfo *
optiondb_findbykey(const struct carg_optiondb *db, int key);


#endif  // OPTIONDB_H_
