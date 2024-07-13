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


struct optiondb {
    const struct carg_option **repo;
    size_t size;
    volatile size_t count;
};


int
optiondb_init(struct optiondb *db);


void
optiondb_dispose(struct optiondb *db);


int
optiondb_insert(struct optiondb *db, const struct carg_option *opt);


int
optiondb_insertvector(struct optiondb *db,
        const struct carg_option *options);


int
optiondb_exists(struct optiondb *db, const struct carg_option *opt);


const struct carg_option *
optiondb_findbyname(const struct optiondb *db, const char *name,
        int len);


const struct carg_option *
optiondb_findbykey(const struct optiondb *db, int key);


#endif  // OPTIONDB_H_
