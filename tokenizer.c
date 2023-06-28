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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <clog.h>

#include "option.h"
#include "tokenizer.h"


static int
tokenize(struct carg_option *options, int argc, char **argv,
        const char **value, struct carg_option **optout) {
    // TODO: allocate options state nargs [len(options)]

    /* Coroutine  stuff*/
    #define CSTART \
        static int __cline__ = 0; \
        switch (__cline__) { \
            case 0:

    #define CREJECT goto cfinally

    #define CYIELD(v, l, o) do { \
            __cline__ = __LINE__; \
            *value = (v); \
            *optout = o; \
            return (l); \
            case __LINE__:; \
        } while (0)


    #define CEND } cfinally: \
        *value = NULL; \
        *optout = NULL; \
        __cline__ = 0; \
        return 0

    static int i;
    static int j;
    static int toklen;
    static const char *tok;
    struct carg_option *opt = NULL;

    CSTART;
    for (i = 0; i < argc; i++) {
        tok = argv[i];
        DEBUG("tok: %s", tok);
        opt = NULL;

        if (tok == NULL) {
            CREJECT;
        }

        toklen = strlen(tok);
        if (toklen == 0) {
            continue;
        }

        if (toklen == 1) {
            CYIELD(tok, toklen, NULL);
            continue;
        }

        if ((tok[0] == '-') && (tok[1] == '-')) {
            /* Double dashes option: --foo */
            // opt = _findoption_byname(c, tok + 2);
            CYIELD(tok, toklen, opt);
            continue;
        }

        if (tok[0] == '-') {
            /* Single dash option: -f */
            for (j = 1; j < toklen; j++) {
                opt = option_findbykey(options, tok[j]);
                if (opt == NULL) {
                    CYIELD(tok + j, toklen - j, NULL);
                    break;
                }
                CYIELD(tok + j, 1, opt);
            }

            continue;
        }

        /* Positional argument */
        CYIELD(tok, toklen, NULL);
    }

    CEND;
}
