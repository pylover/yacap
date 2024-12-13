// Copyright 2023 Vahid Mardani
/*
 * This file is part of yacap.
 *  yacap is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  yacap is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with yacap. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <clog.h>

#include "suggest.h"
#include "yacap.h"


#define BASH_PATH "/usr/bin/bash"
#define SUGGESTIONS_MAX 8
#define BUFFSIZE 1023
char *_Nullable suggestions[SUGGESTIONS_MAX];
char outbuff[BUFFSIZE + 1];
char errbuff[BUFFSIZE + 1];


#define SCRIPT \
    "echo Helloccccccc"


static void
_pipeclose(int p[2]) {
    int i;

    for (i = 0; i < 2; i++) {
        if (p[i] > -1) {
            close(p[i]);
        }
    }
}


static int
_child() {
    char *argv[] = { NULL };
    char *env[] = { NULL };
    if (execve(BASH_PATH, argv, env) == -1) {
        return -1;
    }

    exit(EXIT_SUCCESS);
}


int
suggest(struct yacap *y, const char *userinput) {
    int rbytes;
    int pipein[2] = {-1, -1};
    int pipeout[2] = {-1, -1};
    int pipeerr[2] = {-1, -1};
    pid_t cpid;
    pid_t w;
    int wstatus;

    if (pipe(pipein)) {
        goto failed;
    }

    if (pipe(pipeout)) {
        goto failed;
    }

    if (pipe(pipeerr)) {
        goto failed;
    }

    cpid = fork();
    if (cpid == -1) {
        goto failed;
    }

    if (cpid) {
        /* Parrent process */
        DEBUG("child pid: %d", cpid);

        /* close the read side of the stdin pipe and also write side of the
         * stdout and stderr pipes. */
        close(pipein[0]);
        close(pipeout[1]);
        close(pipeerr[1]);

        dprintf(pipein[1], "/usr/bin/echo hello stdout;\n");
        dprintf(pipein[1], "/usr/bin/echo hello stderr >&2;\n");
        close(pipein[1]);

        /* read from stdout */
        rbytes = read(pipeout[0], outbuff, BUFFSIZE);
        if (rbytes == -1) {
            goto failed;
        }
        outbuff[rbytes] = '\0';

        /* read from stderr */
        rbytes = read(pipeerr[0], errbuff, BUFFSIZE);
        if (rbytes == -1) {
            goto failed;
        }
        errbuff[rbytes] = '\0';

        do {
            w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
            if (w == -1) {
                ERROR("waitpid");
                goto failed;
            }

            if (WIFEXITED(wstatus)) {
                INFO("child process: %d exited, status=%d", cpid,
                        WEXITSTATUS(wstatus));
            }
            else if (WIFSIGNALED(wstatus)) {
                WARN("child process: %d killed by signal %d",  cpid,
                        WTERMSIG(wstatus));
            }
            else if (WIFSTOPPED(wstatus)) {
                WARN("child process: %d stopped by signal %d", cpid,
                        WSTOPSIG(wstatus));
            }
            else if (WIFCONTINUED(wstatus)) {
                INFO("child process: %d continued", cpid);
            }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
    }
    else {
        /* Child process */

        /* close the write side of the stdin pipe and read side of the stdout
         * and stderr pipes. */
        close(pipein[1]);
        close(pipeout[0]);
        close(pipeerr[0]);

        /* replace stdin with the read side of the input pipe */
        if (dup2(pipein[0], STDIN_FILENO) == -1) {
            ERROR("dup2 stdin");
            goto childfailed;
        }

        /* replace stdout with the write side of the output pipe */
        if (dup2(pipeout[1], STDOUT_FILENO) == -1) {
            ERROR("dup2 stdout");
            goto childfailed;
        }

        /* replace stderr with the write side of the error pipe */
        if (dup2(pipeerr[1], STDERR_FILENO) == -1) {
            ERROR("dup2 stderr");
            goto childfailed;
        }

        /* execute script */
        exit(_child());

childfailed:
        exit(EXIT_FAILURE);
    }

    _pipeclose(pipein);
    _pipeclose(pipeout);
    _pipeclose(pipeerr);
    return 0;

failed:
    _pipeclose(pipein);
    _pipeclose(pipeout);
    _pipeclose(pipeerr);
    return -1;
}
