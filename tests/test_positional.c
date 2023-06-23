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


struct foobarbaz {
    const char* foo;
    const char* bar;
    const char* baz;
};


static enum carg_eatstatus
eat_foobarbaz(int key, const char *value, struct carg_state *state) {
    struct foobarbaz *a = state->userptr;

    if (a == NULL) {
        return CARG_EAT_UNRECOGNIZED;
    }

    if (key == CARG_POSITIONAL) {
        switch (state->posindex) {
            case 0:
                a->foo = value;
                return CARG_EAT_OK;

            case 1:
                a->bar = value;
                return CARG_EAT_OK;

            case 2:
                a->baz = value;
                return CARG_EAT_OK;
        }
    }
    else if (key == CARG_END) {
        if (a->baz == NULL) {
            return CARG_EAT_ARG_INSUFFICIENT;
        }
        return CARG_EAT_OK;
    }
    return CARG_EAT_UNRECOGNIZED;
}


static void
test_positionals() {
    struct foobarbaz args = {NULL, NULL, NULL};
    struct carg carg = {
        .eat = eat_foobarbaz,
        .options = nooption,
        .args = NULL,
        .header = NULL,
        .footer = NULL,
        .version = NULL,
    };

    args.foo = NULL;
    args.bar = NULL;
    args.baz = NULL;
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar baz", &args));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foo);
    eqstr("bar", args.bar);
    eqstr("baz", args.baz);

    args.foo = NULL;
    args.bar = NULL;
    args.baz = NULL;
    eqint(CARG_ERR, carg_parse_string(&carg, "qux foo bar baz thud", &args));
    eqstr("", out);
    eqstr("qux: Invalid argument: thud\n"
        "Try `qux --help' or `qux --usage' for more information.\n", err);

    args.foo = NULL;
    args.bar = NULL;
    args.baz = NULL;
    eqint(CARG_ERR, carg_parse_string(&carg, "qux foo bar", &args));
    eqstr("", out);
    eqstr("qux: insufficient argument(s)\n"
        "Try `qux --help' or `qux --usage' for more information.\n", err);
}


struct fooargs {
    const char* foos[8];
    int count;
    const char *bar;
    const char *baz;
};


static enum carg_eatstatus
eat_fooargs(int key, const char *value, struct carg_state *state) {
    struct fooargs *a = state->userptr;

    if (a == NULL) {
        return CARG_EAT_UNRECOGNIZED;
    }

    if (key == 'b') {
        a->bar = value;
        return CARG_EAT_OK;
    }
    else if (key == 'z') {
        a->baz = value;
        return CARG_EAT_OK;
    }
    else if (key == CARG_POSITIONAL) {
        a->foos[a->count++] = value;
        return CARG_EAT_OK;
    }
    return CARG_EAT_UNRECOGNIZED;
}

static void
test_dashdash() {
    struct fooargs args;
    struct carg_option options[] = {
        {"bar", 'b', "BAR", 0, NULL},
        {"baz", 'z', "BAZ", 0, NULL},
        {NULL}
    };
    struct carg carg = {
        .eat = eat_fooargs,
        .options = options,
        .args = NULL,
        .header = NULL,
        .footer = NULL,
        .version = NULL,
    };

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar baz", &args));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foos[0]);
    eqstr("bar", args.foos[1]);
    eqstr("baz", args.foos[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo -- --bar -zbaz -- quux",
                &args));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foos[0]);
    eqstr("--bar", args.foos[1]);
    eqstr("-zbaz", args.foos[2]);
    eqstr("--", args.foos[3]);
    eqstr("quux", args.foos[4]);
    eqint(5, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux -- foo bar baz", &args));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foos[0]);
    eqstr("bar", args.foos[1]);
    eqstr("baz", args.foos[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo -- bar baz", &args));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foos[0]);
    eqstr("bar", args.foos[1]);
    eqstr("baz", args.foos[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar -- baz", &args));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foos[0]);
    eqstr("bar", args.foos[1]);
    eqstr("baz", args.foos[2]);
    eqint(3, args.count);

    memset(&args, 0, sizeof(args));
    eqint(CARG_OK, carg_parse_string(&carg, "qux foo bar baz --", &args));
    eqstr("", out);
    eqstr("", err);
    eqstr("foo", args.foos[0]);
    eqstr("bar", args.foos[1]);
    eqstr("baz", args.foos[2]);
    eqint(3, args.count);
}


int
main() {
    test_positionals();
    test_dashdash();
    return EXIT_SUCCESS;
}
