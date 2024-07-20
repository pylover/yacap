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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"
#include "internal.h"
#include "option.h"
#include "optiondb.h"


#define EXTENDSIZE 8


int
optiondb_extend(struct optiondb *db) {
    struct optioninfo *new;
    size_t newsize = db->size + EXTENDSIZE;

    if (newsize > CARG_OPTIONS_MAX) {
        newsize = CARG_OPTIONS_MAX;
    }

    if (newsize <= db->size) {
        PERR("maximum allowed options are exceeded: %d\n", CARG_OPTIONS_MAX);
        return -1;
    }

    new = realloc(db->repo, newsize * sizeof(struct optioninfo));

    if (new == NULL) {
        return -1;
    }

    db->repo = new;
    db->size = newsize;
    return 0;
}


int
optiondb_exists(struct optiondb *db, const struct carg_option *opt) {
    int i;
    const struct optioninfo *info;

    for (i = 0; i < db->count; i++) {
        info = db->repo + i;
        if (((info->option->key == opt->key) ||
                 (info->option->name && opt->name &&
                  CMP(info->option->name, opt->name)))) {
            return 1;
        }
    }

    return 0;
}


int
optiondb_insert(struct optiondb *db, const struct carg_option *opt,
        const struct carg_command *command) {
    struct optioninfo *info;

    /* check existance */
    if (optiondb_exists(db, opt)) {
        PERR("option duplicated -- '");
        option_print(STDERR_FILENO, opt);
        PERR("'\n");
        return -1;
    }

    /* extend db if there is no space for new item */
    if ((db->count == db->size) && optiondb_extend(db)) {
        return -1;
    }

    info = db->repo + (db->count++);
    info->option = opt;
    info->command = command;
    return 0;
}


int
optiondb_insertvector(struct optiondb *db, const struct carg_option *opt,
        const struct carg_command *cmd) {
    if (opt == NULL) {
        return 0;
    }

    while (opt && opt->name) {
        if (optiondb_insert(db, opt++, cmd)) {
            return -1;
        }
    }

    return 0;
}


int
optiondb_init(struct optiondb *db) {
    db->repo = calloc(EXTENDSIZE, sizeof(struct optioninfo));
    if (db->repo == NULL) {
        return -1;
    }
    db->size = EXTENDSIZE;
    db->count = 0;

    return 0;
}


void
optiondb_dispose(struct optiondb *db) {
    if (db->repo) {
        free(db->repo);
    }

    db->count = -1;
}


const struct optioninfo *
optiondb_findbyname(const struct optiondb *db, const char *name,
        int len) {
    int i;
    const struct optioninfo *info;

    if (name == NULL) {
        return NULL;
    }

    for (i = 0; i < db->count; i++) {
        info = db->repo + i;

        if (info->option->name == NULL) {
            continue;
        }

        if (CMPN(name, info->option->name, len)) {
            return info;
        }
    }

    return NULL;
}


const struct optioninfo *
optiondb_findbykey(const struct optiondb *db, int key) {
    int i;
    const struct optioninfo *info;

    for (i = 0; i < db->count; i++) {
        info = db->repo + i;

        if (info->option->key == key) {
            return info;
        }
    }

    return NULL;
}
