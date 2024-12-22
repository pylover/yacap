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
#include "completionscript.h"


static int
_header_write(int fd, struct yacap *y) {
    // c->printf(
    return -1;
}


static int
_commandsfunc_write(int fd, struct yacap *y) {
    // TODO: implement
    return -1;
}


static int
_mainfunc_write(int fd, struct yacap *y) {
    // TODO: implement
    return -1;
}


int
completionscript_write(int fd, struct yacap *y) {
    if (_header_write(fd, y)) {
        return -1;
    }

    if (_commandsfunc_write(fd, y)) {
        return -1;
    }

    if (_mainfunc_write(fd, y)) {
        return -1;
    }

    return 0;
}
