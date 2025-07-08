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
#ifndef OPTIONDB_H_
#define OPTIONDB_H_


#include <stddef.h>

#include "include/yacap.h"


enum optioninfoflag {
    OPT_NONE = 0,
    OPT_UNIQUE = 1,
};


struct optioninfo {
    const struct yacap_option *option;
    const struct yacap_command *command;
    unsigned int occurances;
};


struct optiondb {
    struct optioninfo *repo;
    size_t size;
    volatile size_t count;
};


int
optiondb_init(struct optiondb *db);


void
optiondb_dispose(struct optiondb *db);


int
optiondb_insert(struct optiondb *db, const struct yacap_option *opt,
        const struct yacap_command *command);


int
optiondb_insertvector(struct optiondb *db, const struct yacap_option *opt,
        const struct yacap_command *cmd);


int
optiondb_exists(struct optiondb *db, const struct yacap_option *opt);


struct optioninfo *
optiondb_findbyname(const struct optiondb *db, const char *name,
        int len);


struct optioninfo *
optiondb_findbykey(const struct optiondb *db, int key);


#endif  // OPTIONDB_H_
