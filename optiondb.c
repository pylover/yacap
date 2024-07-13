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

#include "option.h"
#include "optiondb.h"


#define CMP(x, y) (strcmp(x, y) == 0)
#define CMPN(x, y, l) (strncmp(x, y, l) == 0)
#define EXTENDSIZE 8


int
optiondb_extend(struct carg_optiondb *db) {
    const struct carg_option **new;

    new = realloc(db->repo,
            (db->size + EXTENDSIZE) * sizeof(struct carg_option*));

    if (new == NULL) {
        return -1;
    }

    db->repo = new;
    db->size += EXTENDSIZE;
    return 0;
}


int
optiondb_exists(struct carg_optiondb *db, const struct carg_option *opt) {
    int i;
    const struct carg_option *o;

    for (i = 0; i < db->count; i++) {
        o = db->repo[i];
        if ((o->key == opt->key) || (
                    o->name && opt->name && CMP(o->name, opt->name))
                ) {
            return 1;
        }
    }

    return 0;
}


int
optiondb_insert(struct carg_optiondb *db, const struct carg_option *opt) {
    /* check existance */
    if (optiondb_exists(db, opt)) {
        dprintf(STDERR_FILENO, "[carg] option duplicated -- '%s'\n",
                option_repr(opt));
        return -1;
    }

    /* extend db if there is no space for new item */
    if ((db->count == db->size) && optiondb_extend(db)) {
        return -1;
    }

    db->repo[db->count] = opt;
    db->count++;
    return 0;
}


int
optiondb_insertvector(struct carg_optiondb *db,
        const struct carg_option *opt) {
    while (opt && opt->name) {
        if (optiondb_insert(db, opt++)) {
            return -1;
        }
    }

    return 0;
}


int
optiondb_init(struct carg_optiondb *db) {
    db->repo = calloc(EXTENDSIZE, sizeof (struct carg_option*));
    if (db->repo == NULL) {
        return -1;
    }
    db->size = EXTENDSIZE;
    db->count = 0;

    return 0;
}


void
optiondb_dispose(struct carg_optiondb *db) {
    if (db->repo) {
        free(db->repo);
    }

    db->count = -1;
}


const struct carg_option *
optiondb_findbyname(const struct carg_optiondb *db, const char *name,
        int len) {
    int i;
    const struct carg_option *opt;

    if (name == NULL) {
        return NULL;
    }

    for (i = 0; i < db->count; i++) {
        opt = db->repo[i];

        if (opt->name == NULL) {
            continue;
        }

        if (CMPN(name, opt->name, len)) {
            return opt;
        }
    }

    return NULL;
}


const struct carg_option *
optiondb_findbykey(const struct carg_optiondb *db, int key) {
    int i;
    const struct carg_option *opt;

    for (i = 0; i < db->count; i++) {
        opt = db->repo[i];

        if (opt->key == key) {
            return opt;
        }
    }

    return NULL;
}
