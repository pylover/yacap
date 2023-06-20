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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

#include <cutest.h>
#include <clog.h>
#include "carg.h"


/*
Usage: wepn [OPTION...] serve
  or:  wepn [OPTION...] connect <HOST>
  or:  wepn [OPTION...] token list
  or:  wepn [OPTION...] token create
  or:  wepn [OPTION...] token delete <INDEX>

Simple Virtual Private Network

  -c, --configuration-file=FILENAME
                             Configuration filename, default:
                             /etc/wepn/wepn.conf (if exists)
  -v, --verbosity=LEVEL      Verbosity level: 0-5. default: 4
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to http://github.com/pylover/wepn.
*/


struct carg_option options[] = {
    { NULL }
};


#define HELP ""


void
test_carg() {
    struct carg carg = {
        .doc = "Lorem ipsum indit cunfto",
        .options = options,
        .footer = "Lorem ipsum footer"
    };
    
    int outpipe[2];
    int errpipe[2];
    char out[1024] = "\0";
    char err[1024] = "\0";
    pipe(outpipe);
    pipe(errpipe);

    carg_outfile_set(outpipe[1]);
    carg_errfile_set(errpipe[1]);
    carg_parse_string(&carg, "--help");
    
    close(outpipe[1]);
    close(errpipe[1]);
    read(outpipe[0], out, 1023);
    read(errpipe[0], err, 1023);
    eqstr(HELP, out);
    eqstr("", err);
    // eqint(10, state.all);
    // eqint(5, state.foo);
    // eqint(5, state.bar);
}


int
main() {
    test_carg();
    return EXIT_SUCCESS;
}
