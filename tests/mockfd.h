#ifndef TESTS_MOCKFD_H_
#define TESTS_MOCKFD_H_


#include <stddef.h>


enum mockfd_direction {
    /* input means parent to child */
    MFDD_IN,
    /* output means child to parent */
    MFDD_OUT,
};


enum mockfd_bitflags {
    MFDF_CHILD_NONBLOCK = 1,
    MFDF_PARENT_NONBLOCK = 2,
    MFDF_UNBUFFERED = 4,
    MFDF_USERBUFFERS = 8,
};


struct mockfd {
    int fd;
    enum mockfd_direction direction;
    int backupfd;
    int pipe[2];
    char *buff;
    size_t buffsize;
    size_t bufflen;
    enum mockfd_bitflags flags;
};


#define MFD_PIPEFD_CHILD(m) (m)->pipe[(int)((m)->direction == MFDD_OUT)]
#define MFD_PIPEFD_PARENT(m) (m)->pipe[(int)((m)->direction == MFDD_IN)]


int
mockfd_init(struct mockfd *mfd, int fd, enum mockfd_direction dir,
        size_t buffsize, int flags);


int
mockfd_deinit(struct mockfd *mfd);


int
mockfd_child_replace(struct mockfd *mfd);


int
mockfd_child_restore(struct mockfd *mfd);


void
mockfd_parent_prepare(struct mockfd *mfd);


int
mockfd_parent_perform(struct mockfd *mfd);


#endif  // TESTS_MOCKFD_H_
