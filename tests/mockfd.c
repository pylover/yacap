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
}


int
mockfd_init(struct mockfd *mfd, int fd, enum mockfd_direction dir,
        size_t buffsize, int flags) {
    if (fd < 0) {
        errno = EINVAL;
        return -1;
    }

    mfd->flags = flags;

    if (!(flags & MFDF_NOBUFF)) {
        if (buffsize <= 0) {
            errno = EINVAL;
            return -1;
        }

        mfd->buffsize = buffsize;
        mfd->bufflen = 0;

        mfd->buff = malloc(buffsize + 1);
        if (mfd->buff == NULL) {
            return -1;
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
        (fcntl(MFD_CHILDSIDE(mfd), F_SETFL, O_NONBLOCK) == -1)) {
        goto failed;
    }

    if ((flags & MFDF_PARENT_NONBLOCK) &&
        (fcntl(MFD_PARENTSIDE(mfd), F_SETFL, O_NONBLOCK) == -1)) {
        goto failed;
    }

    return 0;

failed:
    if (!(flags & MFDF_NOBUFF)) {
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
    if (dup2(MFD_CHILDSIDE(mfd), mfd->fd) == -1) {
        mfd->backupfd = -1;
        return -1;
    }

    /* close the other(parent) side of the pipe */
    close(mfd->pipe[MFD_PARENTSIDE(mfd)]);
    mfd->pipe[MFD_PARENTSIDE(mfd)] = -1;
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
    close(mfd->pipe[MFD_CHILDSIDE(mfd)]);
    mfd->backupfd = -1;
    mfd->pipe[MFD_CHILDSIDE(mfd)] = -1;
    return 0;
}


int
mockfd_parent_perform(struct mockfd *mfd) {
    int ret;
    int parentfd = MFD_PARENTSIDE(mfd);

    if (mfd->flags & MFDF_NOBUFF) {
        ERROR("Cannot perform an unbuffered mockfd");
        return -1;
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

    close(parentfd);
    return 0;
}


int
mockfd_deinit(struct mockfd *mfd) {
    if (mfd == NULL) {
        return -1;
    }

    _pipeclose(mfd);

    if (!(mfd->flags & MFDF_NOBUFF)) {
        if (mfd->buff) {
            free(mfd->buff);
        }

        mfd->buffsize = 0;
        mfd->bufflen = 0;
        mfd->buff = NULL;
    }

    mfd->pipe[0] = -1;
    mfd->pipe[1] = -1;
    mfd->fd = -1;
    mfd->backupfd = -1;

    return 0;
}
