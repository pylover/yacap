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
mockstd_init(struct mockstd *m, char *stdout_buff, size_t stdout_buffsize,
        char *stderr_buff, size_t stderr_buffsize);


int
mockstd_deinit(struct mockstd *m);


void
mockstd_parent_prepare(struct mockstd *m);


int
mockstd_parent_perform(struct mockstd *m);


int
mockstd_child_replace(struct mockstd *m);


int
mockstd_child_restore(struct mockstd *m);


int
mockstd_parent_write(struct mockstd *m, const char *fmt, ...);


#endif  // TESTS_MOCKSTD_H_
