// Copyright 2023 Vahid Mardani
/*
 * This file is part of CArg.
 *  CArg is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  CArg is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with CArg. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <clog.h>

#include "helpers.h"


char out[BUFFSIZE + 1] = "\0";
char err[BUFFSIZE + 1] = "\0";


static int
_restore_fd(int fd, int newfd, int backupfd) {
    bool failed = false;

    /* flush fd buffer */
    if (fsync(fd) == -1) {
        failed |= true;
    }

    if (dup2(backupfd, fd) == -1) {
        failed |= true;
    }

    if (close(backupfd) == -1) {
        failed |= true;
    }

    if (close(newfd) == -1) {
        failed |= true;
    }

    return !failed;
}


static int
_replace_fd(int fd, int pipe_[2], int *backupfd) {
    int backup = -1;
    pipe_[0] = -1;
    pipe_[1] = -1;

    /* backup the fd using fd */
    backup = dup(fd);
    if (backup == -1) {
        return -1;
    }

    /* create a in-memory file */
    if (pipe(pipe_) == -1) {
        goto failed;
    }

    if (fcntl(pipe_[0], F_SETFL, O_NONBLOCK) == -1) {
        goto failed;
    }

    /* replace the fd with write side of the pipe */
    if (dup2(pipe_[1], fd) == -1) {
        goto failed;
    }

    /* close the write side of the pipe */
    if (close(pipe_[1])) {
        dup2(backup, fd);
        goto failed;
    }

    *backupfd = backup;
    errno = 0;
    return 0;

failed:
    if (backup != -1) {
        close(backup);
    }

    if (pipe_[0] != -1) {
        close(pipe_[0]);
    }

    if (pipe_[1] != -1) {
        close(pipe_[1]);
    }

    errno = 0;
    return -1;
}


enum carg_status
carg_parse_string(struct carg *c, const char * line,
        const struct carg_subcommand **subcommand) {
    char *argv[256];
    int argc = 0;
    char delim[1] = {' '};
    char *needle;
    char *saveptr = NULL;
    static char buff[BUFFSIZE + 1];
    strcpy(buff, line);

    needle = strtok_r(buff, delim, &saveptr);
    argv[argc++] = needle;
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        argv[argc++] = needle;
    }

    /* replacing stdout and stderr temporarily */
    int outpipe[2];
    int errpipe[2];
    int outfd_backup;
    int errfd_backup;

    fflush(stdout);
    if (_replace_fd(STDOUT_FILENO, outpipe, &outfd_backup) == -1) {
        ERROR("_replace_fd");
        return CARG_FATAL;
    }

    fflush(stderr);
    if (_replace_fd(STDERR_FILENO, errpipe, &errfd_backup) == -1) {
        ERROR("_replace_fd");
        _restore_fd(STDOUT_FILENO, outpipe[0], outfd_backup);
        return CARG_FATAL;
    }

    bool failed = false;
    int ret = carg_parse(c, argc, (const char **)argv, subcommand);
    memset(out, 0, BUFFSIZE + 1);
    memset(err, 0, BUFFSIZE + 1);

    if (read(outpipe[0], out, BUFFSIZE) == -1) {
        failed |= true;
    }

    if (read(errpipe[0], err, BUFFSIZE) == -1) {
        failed |= true;
    }

    if (_restore_fd(STDOUT_FILENO, outpipe[0], outfd_backup) == -1) {
        failed |= true;
    }

    if (_restore_fd(STDERR_FILENO, errpipe[0], errfd_backup) == -1) {
        failed |= true;
    }

    carg_dispose(c);
    return ret;
}
