#include <string.h>

#include "option.h"


#define CMP(x, y, l) (strncmp(x, y, l) == 0)


const struct carg_option *
option_findbyname(const struct carg_option *options[], int count,
        const char *name, int len) {
    int i;
    const struct carg_option *opt;

    for (i = 0; i < count; i++) {
        opt = options[i];

        while (opt->name) {
            if (CMP(name, opt->name, len)) {
                return opt;
            }

            opt++;
        }
    }

    return NULL;
}


const struct carg_option *
option_findbykey(const struct carg_option *options[], int count, int key) {
    int i;
    const struct carg_option *opt;

    for (i = 0; i < count; i++) {
        opt = options[i];

        while (opt->name) {
            if (opt->key == key) {
                return opt;
            }

            opt++;
        }
    }

    return NULL;
}
