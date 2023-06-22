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
#include <stdbool.h>

#include "print.h"


#define MAX(x, y) ((x) > (y)? (x): (y))
#define BETWEEN(c, l, u) (((c) >= l) && ((c) <= u))
#define ISCHAR(c) ( \
        BETWEEN(c, 48, 57) || \
        BETWEEN(c, 65, 90) || \
        BETWEEN(c, 97, 122))


void
print_multiline(int fd, const char *string, int indent, int linemax) {
    int remain;
    int linesize = linemax - indent;
    int ls;
    bool dash = false;

    if (string == NULL) {
        return;
    }

    remain = strlen(string);
    while (remain) {
        dash = false;
        while (remain && isspace(string[0])) {
            string++;
            remain--;
        }

        if (remain <= linesize) {
            dprintf(fd, "%s\n", string);
            remain = 0;
            break;
        }

        ls = linesize;
        if (string[ls - 2] == ' ') {
            ls--;
        }
        else if ((string[ls - 1] != ' ') && (string[ls] != ' ')) {
            ls--;
            dash = true;
        }

        dprintf(fd, "%.*s%s\n", ls, string, dash? "-": "");
        remain -= ls;
        string += ls;
        dprintf(fd, "%*s", indent, "");
    }
}


void
print_options(int fd, struct carg *c) {
    int gapsize = 7;
    int i = 0;
    struct carg_option *opt;
    int tmp = 0;

    while (true) {
        opt = &(c->options[i++]);

        if (opt->longname == NULL) {
            break;
        }

        tmp = strlen(opt->longname) + (opt->arg? strlen(opt->arg) + 1: 0);
        gapsize = MAX(gapsize, tmp);
    }
    gapsize += 8;
    char gap[gapsize + 1];
    gap[gapsize] = '\0';
    memset(gap, ' ', gapsize);

    dprintf(fd, "\n");
    i = 0;
    while (true) {
        opt = &(c->options[i++]);

        if (opt->longname == NULL) {
            break;
        }

        if (ISCHAR(opt->key)) {
            dprintf(fd, "  -%c, ", opt->key);
        }
        else {
            dprintf(fd, "      ");
        }

        if (opt->arg == NULL) {
            dprintf(fd, "--%s%.*s", opt->longname,
                    gapsize - ((int)strlen(opt->longname)), gap);
        }
        else {
            tmp = gapsize -
                (int)(strlen(opt->longname) + strlen(opt->arg) + 1);
            dprintf(fd, "--%s=%s%.*s", opt->longname, opt->arg, tmp, gap);
        }

        if (opt->help) {
            print_multiline(fd, opt->help, gapsize + 8, HELP_LINESIZE);
        }
        else {
            dprintf(fd, "\n");
        }
    }
    dprintf(fd, "  -h, --help%.*sGive this help list\n",
            gapsize - 4, gap);
    dprintf(fd, "  -?, --usage%.*sGive a short usage message\n",
            gapsize - 5, gap);

    if (c->version) {
        dprintf(fd, "  -V, --version%.*sPrint program version\n",
                gapsize - 7, gap);
    }

    dprintf(fd, "\n");
}
