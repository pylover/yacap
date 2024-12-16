#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <clog.h>

#include "mockfd.h"


static void
_pipeclose(struct mockfd *mfd) {
    for (int i = 0; i < 2; i++) {
        if (mfd->pipe[i] != -1) {
            close(mfd->pipe[i]);
            mfd->pipe[i] = -1;
        }
    }
    errno = 0;
}


int
mockfd_init(struct mockfd *mfd, int fd, enum mockfd_direction dir,
        size_t buffsize, int flags) {
    if (fd < 0) {
        errno = EINVAL;
        return -1;
    }

    mfd->flags = flags;

    if (!(flags & MFDF_UNBUFFERED)) {
        if (buffsize <= 0) {
            errno = EINVAL;
            return -1;
        }

        mfd->buffsize = buffsize;
        mfd->bufflen = 0;

        if (!(flags & MFDF_USERBUFFERS)) {
            mfd->buff = malloc(buffsize + 1);
            if (mfd->buff == NULL) {
                return -1;
            }
        }
        mfd->buff[0] = '\0';
    }

    mfd->pipe[0] = -1;
    mfd->pipe[1] = -1;
    mfd->fd = fd;
    mfd->backupfd = -1;
    mfd->direction = dir;

    if (pipe(mfd->pipe)) {
        goto failed;
    }

    if ((flags & MFDF_CHILD_NONBLOCK) &&
        (fcntl(MFD_PIPEFD_CHILD(mfd), F_SETFL, O_NONBLOCK) == -1)) {
        goto failed;
    }

    if ((flags & MFDF_PARENT_NONBLOCK) &&
        (fcntl(MFD_PIPEFD_PARENT(mfd), F_SETFL, O_NONBLOCK) == -1)) {
        goto failed;
    }

    return 0;

failed:
    if ((!(flags & MFDF_UNBUFFERED)) && (!(flags & MFDF_USERBUFFERS))) {
        free(mfd->buff);
        mfd->buff = NULL;
    }

    _pipeclose(mfd);
    return -1;
}


/** replace the file descriptor.
 * this function is assumed to be called inside the child process
 */
int
mockfd_child_replace(struct mockfd *mfd) {
    /* backup the fd */
    mfd->backupfd = dup(mfd->fd);
    if (mfd->backupfd == -1) {
        return -1;
    }

    /* perform the actual replacement */
    if (dup2(MFD_PIPEFD_CHILD(mfd), mfd->fd) == -1) {
        mfd->backupfd = -1;
        return -1;
    }

    /* close the other(parent) side of the pipe */
    close(MFD_PIPEFD_PARENT(mfd));
    MFD_PIPEFD_PARENT(mfd) = -1;
    return 0;
}


int
mockfd_child_restore(struct mockfd *mfd) {
    /* flush fd */
    if (fsync(mfd->fd) == -1) {
        return -1;
    }

    if (dup2(mfd->backupfd, mfd->fd) == -1) {
        return -1;
    }

    close(mfd->backupfd);
    close(MFD_PIPEFD_CHILD(mfd));
    MFD_PIPEFD_CHILD(mfd) = -1;
    mfd->backupfd = -1;
    return 0;
}


void
mockfd_parent_prepare(struct mockfd *mfd) {
    close(MFD_PIPEFD_CHILD(mfd));
    MFD_PIPEFD_CHILD(mfd) = -1;
}


int
mockfd_parent_perform(struct mockfd *mfd) {
    int ret;
    int parentfd = MFD_PIPEFD_PARENT(mfd);

    if (mfd->flags & MFDF_UNBUFFERED) {
        /* Cannot perform an unbuffered mockfd */
        goto done;
    }

    if (mfd->direction == MFDD_IN) {
        ret = write(parentfd, mfd->buff, mfd->bufflen);
        if (ret != mfd->bufflen) {
            return -1;
        }

    }
    else {
        ret = read(parentfd, mfd->buff, mfd->buffsize);
        if (ret == -1) {
            return -1;
        }

        mfd->bufflen = ret;
        mfd->buff[ret] = '\0';
    }

done:
    close(parentfd);
    return 0;
}


int
mockfd_deinit(struct mockfd *mfd) {
    if (mfd == NULL) {
        return -1;
    }

    _pipeclose(mfd);
    errno = 0;

    if (!(mfd->flags & MFDF_UNBUFFERED)) {
        if ((!(mfd->flags & MFDF_USERBUFFERS)) && mfd->buff) {
            free(mfd->buff);
            mfd->buff = NULL;
        }

        mfd->buffsize = 0;
        mfd->bufflen = 0;
    }

    mfd->pipe[0] = -1;
    mfd->pipe[1] = -1;
    mfd->fd = -1;
    mfd->backupfd = -1;

    return 0;
}
