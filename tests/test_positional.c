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


#include <clog.h>
#include <cutest.h>

#include "carg.h"
#include "helpers.h"


struct args {
    const char* foo;
    const char* bar;
    const char* baz;
};

static struct args args = {NULL, NULL, NULL};


static enum carg_eatresult
eat(int key, const char *value, struct carg_state *state) {
    struct args *a = state->userptr;

    if (a == NULL) {
        return CARG_NOT_EATEN;
    }

    if (key == CARG_POSITIONAL) {
        switch (state->posindex) {
            case 0:
                a->foo = value;
                return CARG_EATEN;

            case 1:
                a->bar = value;
                return CARG_EATEN;

            case 2:
                a->baz = value;
                return CARG_EATEN;
        }
    }
    return CARG_NOT_EATEN;
}


static void
test_positionals() {
    struct carg carg = {
        .eat = eat,
        .options = nooption,
        .args = NULL,
        .header = NULL,
        .footer = NULL,
        .version = NULL,
    };

    eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar baz", &args));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foo);
    eqstr("bar", args.bar);
    eqstr("baz", args.baz);
}


int
main() {
    test_positionals();
    return EXIT_SUCCESS;
}
