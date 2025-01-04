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
#include <sys/mman.h>
#include <unistd.h>

#include <clog.h>
#include <cutest.h>

#include "yacap.h"
#include "completionscript.h"


static struct yacap_command bar_cmd = {
    .name = "bar",
};


static struct yacap foo = {
    .name = "foo",
    .version = "0.1.0",
    .commands = (const struct yacap_command*[]) {
        &bar_cmd,
        NULL
    },
};


#define HEADER \
    "#! /usr/bin/env bash\n" \
    "# foo completion script version: 0.1.0\n"


#define TMPMAX 1024
static char tmp[TMPMAX + 1];


void
test_completionscript_header(int fd) {
    int bytes = read(fd, tmp, strlen(HEADER));
    eqint(strlen(HEADER), bytes);
    eqnstr(HEADER, tmp, bytes);
}


int
main() {
    clog_verbosity = CLOG_DEBUG2;
    int fd = memfd_create(__func__, 0);
    if (fd == -1) {
        ERROR("memfd_create");
        return EXIT_FAILURE;
    }

    int filesize = completionscript_write(fd, &foo);
    istrue(filesize > 0);
    lseek(fd, 0, SEEK_SET);

    test_completionscript_header(fd);

    close(fd);
    return EXIT_SUCCESS;
}
