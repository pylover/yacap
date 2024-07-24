// Copyright 2023 Vahid Mardani
/*
 * This file is part of CArg.
 *  CArg is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  CArg is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with CArg. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "arghint.h"


#define ISSET(i, b) (i & (1 << (b)))
#define SETBIT(i, b) (i |= (1 << (b)))
#define MAXBITS 32
#define MAXARGS (MAXBITS - 2)
#define ARGSMASK (~(3 << MAXARGS))


int
arghint_validate(size_t count, int pattern) {
    int argspat = pattern & ARGSMASK;

    if (count > MAXARGS) {
        if (ISSET(pattern, 31)) {
            return 0;
        }
        else {
            return -1;
        }
    }

    if (ISSET(argspat, count)) {
        return 0;
    }

    if ((argspat == 0) || (count == 0)) {
        return -1;
    }

    int lastbit = (MAXBITS - __builtin_clz(pattern & ARGSMASK)) - 1;
    if ((count > lastbit) && ISSET(pattern, 31)) {
        return 0;
    }

    return -1;
}


int
arghint_parse(const char *args) {
    char *arghint;
    char *tok;
    size_t toklen;
    char *strtokstate;
    int counter = 0;
    int bits = 0;
    int opens = 0;
    int i;
    char *dots;

    if ((args == NULL) || (strlen(args) == 0)) {
        SETBIT(bits, counter);
        return bits;
    }

    arghint = strdup(args);
    if (arghint == NULL) {
        return -1;
    }

    tok = strtok_r(arghint, " ", &strtokstate);
    if ((tok == NULL) || (strlen(tok) == 0)) {
        free(arghint);
        goto failed;
    }

    do {
        toklen = strlen(tok);
        if (tok[0] == '[') {
            SETBIT(bits, counter);
            opens++;
        }
        counter++;

        i = toklen - 1;
        if ((tok[i] != ']') && (i > 3) && (tok[i - 3] == ']')) {
            i -= 3;
        }
        while ((i > 0) && (tok[i] == ']')) {
            opens--;
            i--;
        }

        dots = strstr(tok, "...");
        if (dots) {
            if ((toklen - (dots - tok)) > 3) {
                goto failed;
            }

            if (tok == dots) {
                SETBIT(bits, counter - 1);
            }
            else {
                SETBIT(bits, counter);
            }

            SETBIT(bits, 31);
            // bits |= 1 << 31;
            goto terminate;
        }

        tok = strtok_r(NULL, " ", &strtokstate);
    } while (tok && strlen(tok));

    if (opens) {
        goto failed;
    }

    SETBIT(bits, counter);

terminate:
    free(arghint);
    return bits;

failed:
    free(arghint);
    return -1;
}
