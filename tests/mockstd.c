#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

#include <clog.h>

#include "mockstd.h"


int
mockstd_init(struct mockstd *m, char *stdout_buff, size_t stdout_buffsize,
        char *stderr_buff, size_t stderr_buffsize) {
    int ret;

    ret = mockfd_init(&m->in, STDIN_FILENO, MFDD_IN, 0, MFDF_UNBUFFERED);
    if (ret == -1) {
        goto failed;
    }

    m->out.buff = stdout_buff;
    ret = mockfd_init(&m->out, STDOUT_FILENO, MFDD_OUT, stdout_buffsize,
            MFDF_USERBUFFERS);
    if (ret == -1) {
        goto failed;
    }

    m->err.buff = stderr_buff;
    ret = mockfd_init(&m->err, STDERR_FILENO, MFDD_OUT, stderr_buffsize,
            MFDF_USERBUFFERS);
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
mockstd_deinit(struct mockstd *m) {
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


void
mockstd_parent_prepare(struct mockstd *m) {
    mockfd_parent_prepare(&m->in);
    mockfd_parent_prepare(&m->out);
    mockfd_parent_prepare(&m->err);
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


int
mockstd_parent_write(struct mockstd *m, const char *fmt, ...) {
    int ret;
    va_list args;

    va_start(args, fmt);
    ret = vdprintf(MFD_PIPEFD_PARENT(&m->in), fmt, args);
    va_end(args);

    return ret;
}
