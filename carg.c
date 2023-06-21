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


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <clog.h>

#include "carg.h"
#include "print.h"


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


void
carg_print_help(struct carg *c, const char *prog) {
    /* Usage */
    print_usage(_outfile, c, prog);

    /* Document */
    if (c->doc) {
        print_multiline(_outfile, c->doc, 0, HELP_LINESIZE);
    }

    /* Options */
    print_options(_outfile, c);

    /* Footer */
    if (c->footer) {
        print_multiline(_outfile, c->footer, 0, HELP_LINESIZE);
    }
}


int
carg_parse(struct carg *c, int argc, char **argv) {
    if (argc < 1) {
        return -1;
    }
    carg_print_help(c, argv[0]);

    return 1;
}
