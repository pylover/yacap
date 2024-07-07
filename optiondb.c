#include <string.h>
#include <stdlib.h>

#include "optiondb.h"


#define CMP(x, y, l) (strncmp(x, y, l) == 0)


static int
_parse_argflags(const struct carg_option *opt) {
    return 0;
}


void
optiondb_insert(struct carg_optiondb *db, const struct carg_option *opt) {
    struct carg_optioninfo *info = db->repo + db->count;

    while ((db->count < db->size) && opt && opt->name) {
        info->option = opt;
        info->flags = _parse_argflags(opt);
        opt++;
        info++;
        db->count++;
    }
}


int
optiondb_init(struct carg_optiondb *db, size_t size) {
    db->repo = calloc(size, sizeof (struct carg_optioninfo));
    if (db->repo == NULL) {
        return -1;
    }
    db->size = size;
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

        if (CMP(name, optinfo->option->name, len)) {
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
