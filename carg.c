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

#include <clog.h>

#include "carg.h"


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


static void
_print_options(struct carg *c) {
    int gapsize = 21;
    char gap[gapsize + 1];
    int i = 0;
    struct carg_option *opt;

    gap[gapsize] = '\0';
    memset(gap, ' ', gapsize);

    while (true) {
        opt = &(c->options[i]);
        
        if (opt->longname == NULL) {
            break;
        }
    }
    dprintf(_outfile, "  -h, --help%.*sGive this help list\n", 
            gapsize - 4, gap);
    dprintf(_outfile, "  -?, --usage%.*sGive a short usage message\n", 
            gapsize - 5, gap); 
    dprintf(_outfile, "  -V, --version%.*sPrint program version\n", 
            gapsize - 7, gap); 
    dprintf(_outfile, "\n");
}


void
carg_print_help(struct carg *c, const char *prog) {
    /* Usage */
    dprintf(_outfile, "Usage: %s [OPTIONS]", prog);
    if (c->args) {
        dprintf(_outfile, " %s\n", c->args);
    }
    dprintf(_outfile, "\n");

    /* Document */
    if (c->doc) {
        dprintf(_outfile, "%s\n\n", c->doc);
    }

    /* Options */
    _print_options(c);
    
    /* Footer */
    if (c->footer) {
        dprintf(_outfile, "%s\n", c->footer);
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
