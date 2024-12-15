#include <unistd.h>

#include "mockstd.h"


int
mockstd_init(struct mockstd *m, size_t stdin_buffsize, size_t stdout_buffsize,
        size_t stderr_buffsize) {
    int ret;

    ret = mockfd_init(&m->in, STDIN_FILENO, MFDD_IN, stdin_buffsize, 0);
    if (ret == -1) {
        goto failed;
    }

    ret = mockfd_init(&m->out, STDOUT_FILENO, MFDD_OUT, stdout_buffsize, 0);
    if (ret == -1) {
        goto failed;
    }

    ret = mockfd_init(&m->err, STDOUT_FILENO, MFDD_OUT, stderr_buffsize, 0);
    if (ret == -1) {
        goto failed;
    }

    return 0;

failed:
    mockfd_deinit(&m->in);
    mockfd_deinit(&m->out);
    mockfd_deinit(&m->err);

    return -1;
}


int
moclstd_deinit(struct mockstd *m) {
    int ret = 0;

    if (mockfd_deinit(&m->in)) {
        ret |= 1;
    }

    if (mockfd_deinit(&m->out)) {
        ret |= 1;
    }

    if (mockfd_deinit(&m->err)) {
        ret |= 1;
    }

    return ret;
}


int
mockstd_parent_perform(struct mockstd *m) {
    if (mockfd_parent_perform(&m->in)) {
        return -1;
    }

    if (mockfd_parent_perform(&m->out)) {
        return -1;
    }

    if (mockfd_parent_perform(&m->err)) {
        return -1;
    }

    return 0;
}


int
mockstd_child_replace(struct mockstd *m) {
    if (mockfd_child_replace(&m->in)) {
        return -1;
    }

    if (mockfd_child_replace(&m->out)) {
        mockfd_child_restore(&m->in);
        return -1;
    }

    if (mockfd_child_replace(&m->err)) {
        mockfd_child_restore(&m->in);
        mockfd_child_restore(&m->out);
        return -1;
    }

    return 0;
}


int
mockstd_child_restore(struct mockstd *m) {
    int ret = 0;

    if (mockfd_child_restore(&m->in)) {
        ret |= 1;
    }

    if (mockfd_child_restore(&m->out)) {
        ret |= 1;
    }

    if (mockfd_child_restore(&m->err)) {
        ret |= 1;
    }

    return ret;
}
