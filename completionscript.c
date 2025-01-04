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
#include <stdio.h>
#include <errno.h>

#include "completionscript.h"


#define l(s) s"\n"


static int
_header_write(int fd, struct yacap *y) {
    int ret = 0;
    errno = 0;

    ret += dprintf(fd, "#! /usr/bin/env bash\n");
    ret += dprintf(fd, "# %s completion script version: %s\n", y->name,
            y->version);
    ret += dprintf(fd, "\n");

    if (errno) {
        return -1;
    }
    return ret;
}


// static int
// _commandsfunc_write(int fd, struct yacap *y) {
//     // TODO: implement
//     return -1;
// }


static int
_mainfunc_write(int fd, struct yacap *y) {
    int ret = 0;
    errno = 0;

    ret += dprintf(fd,
        l("__%s_main () {")
        l("  echo \"foo bar\"                                               ")
        l("}                                                                "),
        y->name
    );

    if (errno) {
        return -1;
    }
    return ret;
}


static int
_completionfunc_write(int fd, struct yacap *y) {
    int ret = 0;
    errno = 0;

    ret += dprintf(fd,
        l("__%s () {")
        l("  local cur prev words cword                                     ")
        l("  _get_comp_words_by_ref -n '=:' cur prev words cword            ")
        l("  echo \"${cur}, ${prev}, ${words[*]}, ${cword}\" >> /tmp/gg     ")
        l("  echo wbrk: ${COMP_WORDBREAKS} >> /tmp/gg                       ")
        l("  COMPREPLY=(${words[*]})                                        ")
        l("} && complete -F __%s %s                                         "),
        y->name,
        y->name,
        y->name
    );

    if (errno) {
        return -1;
    }
    return ret;
}


int
completionscript_write(int fd, struct yacap *y) {
    int ret = 0;
    int bytes;

    bytes = _header_write(fd, y);
    if (bytes == -1) {
        return -1;
    }
    ret += bytes;

    bytes = _mainfunc_write(fd, y);
    if (bytes == -1) {
        return -1;
    }
    ret += bytes;

    bytes = _completionfunc_write(fd, y);
    if (bytes == -1) {
        return -1;
    }
    ret += bytes;

    return ret;
}
