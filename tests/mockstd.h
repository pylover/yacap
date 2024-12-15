#ifndef TESTS_MOCKSTD_H_
#define TESTS_MOCKSTD_H_


#include "mockfd.h"


struct mockstd {
    struct mockfd in;
    struct mockfd out;
    struct mockfd err;
    int flags;
};


int
mockstd_init(struct mockstd *m, size_t stdin_buffsize, size_t stdout_buffsize,
        size_t stderr_buffsize);


int
moclstd_deinit(struct mockstd *m);


int
mockstd_parent_perform(struct mockstd *m);


int
mockstd_child_replace(struct mockstd *m);


int
mockstd_child_restore(struct mockstd *m);


#endif  // TESTS_MOCKSTD_H_
