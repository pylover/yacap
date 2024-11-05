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
#ifndef INTERNAL_H_
#define INTERNAL_H_

#include <limits.h>

#include "config.h"
#include "yacap.h"
#include "cmdstack.h"
#include "optiondb.h"


#define CMP(x, y) (strcmp(x, y) == 0)
#define CMPN(x, y, l) (strncmp(x, y, l) == 0)
#define MAX(x, y) ((x) > (y)? (x): (y))
#define HASFLAG(o, f) ((o)->flags & (f))
#define BETWEEN(c, l, u) (((c) >= l) && ((c) <= u))
#define ISSIGN(c) (\
        BETWEEN(c, 32, 47) || \
        BETWEEN(c, 58, 64) || \
        BETWEEN(c, 123, 126))
#define ISDIGIT(c) BETWEEN(c, 48, 57)
#define ISCHAR(c) ((c == '?') || ISDIGIT(c) || \
        BETWEEN(c, 65, 90) || \
        BETWEEN(c, 97, 122))


#define PERR(...) dprintf(STDERR_FILENO, __VA_ARGS__)
#define POUT(...) dprintf(STDOUT_FILENO, __VA_ARGS__)


extern const struct yacap_option opt_verbosity;
extern const struct yacap_option opt_verboseflag;
extern const struct yacap_option opt_quietflag;
extern const struct yacap_option opt_version;
extern const struct yacap_option opt_help;
extern const struct yacap_option opt_usage;


struct yacap_state {
    struct cmdstack cmdstack;
    struct optiondb optiondb;
    size_t positionals;
};


#endif  // INTERNAL_H_
