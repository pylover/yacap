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
#include "mockstd.h"
#include "yacap.h"


#define BASH_PATH "/usr/bin/bash"
#define SUGGESTIONS_MAX 8
#define BUFFSIZE 1023
char sugout[BUFFSIZE + 1];
char sugerr[BUFFSIZE + 1];
struct mockstd mockstd;


static int
_child() {
    char *argv[] = { "" };
    char *env[] = { NULL };
    if (execve(BASH_PATH, argv, env) == -1) {
        return -1;
    }

    exit(EXIT_SUCCESS);
}


static
int
_wait(pid_t cpid) {
    pid_t w;
    int wstatus;

    do {
        w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
        if (w == -1) {
            ERROR("waitpid");
            return -1;
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

    return 0;
}


// static int
// _subprocess_stdin_write() {
// }


int
suggest(struct yacap *y, const char *userinput) {
    pid_t cpid;

    mockstd_init(&mockstd, sugout, BUFFSIZE, sugerr, BUFFSIZE);
    cpid = fork();
    if (cpid == -1) {
        goto failed;
    }

    if (cpid) {
        /* Parrent process */

        mockstd_parent_prepare(&mockstd);
        mockstd_parent_write(&mockstd, "/usr/bin/echo hello stdout;\n");
        mockstd_parent_write(&mockstd, "/usr/bin/echo hello stderr >&2;\n");
        mockstd_parent_perform(&mockstd);

        if (_wait(cpid)) {
            ERROR("wait(pid: %d)", cpid);
            goto failed;
        }

    }
    else {
        /* Child process */

        /* replace standard files */
        mockstd_child_replace(&mockstd);

        /* execute script */
        int cstatus = _child();

        /* restore standard files */
        mockstd_child_restore(&mockstd);
        mockstd_deinit(&mockstd);
        exit(cstatus);
    }

    mockstd_deinit(&mockstd);
    return 0;

failed:
    mockstd_deinit(&mockstd);
    return -1;
}
