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
#include "completionscript.h"
#include "mockstd.h"
#include "yacap.h"


#define BASH_PATH "/usr/bin/bash"
#define SUGGESTIONS_MAX 8
#define BUFFSIZE 1023
char sugout[BUFFSIZE + 1];
char sugerr[BUFFSIZE + 1];
struct mockstd mockstd;


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


static int
_child(const char *userinput, int cursor) {
    char _Nullable pbuff[16];
    char _Nullable lbuff[BUFFSIZE + 1];

    sprintf(pbuff, "COMP_POINT=%d", cursor);
    sprintf(lbuff, "COMP_LINE=\"%s\"", userinput);
    char *_Nullable argv[] = {"-s", NULL};
    char *_Nullable env[] = {
        pbuff,
        lbuff,
        "COMP_WORDBREAKS=\"'><=;|&(:",
        "COMP_CWORD=3",
        "COMP_WORDS=(foo bar baz)",
        NULL
    };

    if (execve(BASH_PATH, argv, env) == -1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


static int
_parent(struct yacap *y) {
    int fd = mockstd_parent_stdin_fileno(&mockstd);

    mockstd_parent_prepare(&mockstd);
    dprintf(fd, "source /etc/bash_completion\n");
    completionscript_write(fd, y);
    dprintf(fd, "__foo_main\n");

    if (mockstd_parent_perform(&mockstd)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


int
suggest(struct yacap *y, const char *userinput) {
    int ret = 0;
    pid_t cpid;

    if (mockstd_init(&mockstd, sugout, BUFFSIZE, sugerr, BUFFSIZE)) {
        return -1;
    }

    cpid = fork();
    if (cpid == -1) {
        ret = -1;
        goto fine;
    }

    /* Parrent process */
    if (cpid) {
        /* read/write to/from standard in/out of the child */
        ret = _parent(y);

        /* Wait for child process to finish */
        ret |= _wait(cpid);
    }
    /* Child process */
    else {
        /* replace standard files */
        mockstd_child_replace(&mockstd);

        /* execute script inside the child prcess */
        ret = _child(userinput, strlen(userinput) - 1);

        /* restore standard files */
        mockstd_child_restore(&mockstd);
    }

fine:
    mockstd_deinit(&mockstd);
    return ret;
}
