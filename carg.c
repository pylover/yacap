// Copyright 2023 Vahid Mardani
/*
 * This file is part of Carrow.
 *  Carrow is free software: you can redistribute it and/or modify it under 
 *  the terms of the GNU General Public License as published by the Free 
 *  Software Foundation, either version 3 of the License, or (at your option) 
 *  any later version.
 *  
 *  Carrow is distributed in the hope that it will be useful, but WITHOUT ANY 
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 *  details.
 *  
 *  You should have received a copy of the GNU General Public License along 
 *  with Carrow. If not, see <https://www.gnu.org/licenses/>. 
 *  
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */


#include "carg.h"


#include <unistd.h>
#include <stdio.h>


static int _outfile = STDOUT_FILENO;
static int _errfile = STDERR_FILENO;


void
carg_outfile_set(int fd) {
    _outfile = fd;
}


void
carg_errfile_set(int fd) {
    _errfile = fd;
}


int
carg_parse_string(struct carg *c, const char *string) {
    return -1;
    dprintf(_outfile, "hey");
}
