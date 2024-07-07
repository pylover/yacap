#include <string.h>
#include <stdlib.h>

#include "optiondb.h"


#define CMP(x, y) (strcmp(x, y) == 0)
#define CMPN(x, y, l) (strncmp(x, y, l) == 0)
#define EXTENDSIZE 8


static int
_parse_argflags(const struct carg_option *opt) {
    return 0;
}


int
optiondb_extend(struct carg_optiondb *db) {
    struct carg_optioninfo *new;
    new = realloc(db->repo,
            (db->size + EXTENDSIZE) * sizeof(struct carg_optioninfo));

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
        o = db->repo[i].option;
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
    struct carg_optioninfo *info;

    while (opt && opt->name) {
        /* check existance */
        if (optiondb_exists(db, opt)) {
            return -1;
        }

        /* extend db if there is no space for new item */
        if ((db->count == db->size) && optiondb_extend(db)) {
            return -1;
        }

        info = db->repo + db->count;
        info->option = opt;
        info->flags = _parse_argflags(opt);
        opt++;
        db->count++;
    }

    return 0;
}


int
optiondb_init(struct carg_optiondb *db) {
    db->repo = calloc(EXTENDSIZE, sizeof (struct carg_optioninfo));
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


const struct carg_optioninfo *
optiondb_findbyname(const struct carg_optiondb *db, const char *name,
        int len) {
    int i;
    const struct carg_optioninfo *optinfo;

    for (i = 0; i < db->count; i++) {
        optinfo = &db->repo[i];

        if (CMPN(name, optinfo->option->name, len)) {
            return optinfo;
        }
    }

    return NULL;
}


const struct carg_optioninfo *
optiondb_findbykey(const struct carg_optiondb *db, int key) {
    int i;
    const struct carg_optioninfo *optinfo;

    for (i = 0; i < db->count; i++) {
        optinfo = &db->repo[i];

        if (optinfo->option->key == key) {
            return optinfo;
        }
    }

    return NULL;
}
