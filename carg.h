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
#ifndef CARG_H_
#define CARG_H_


#include <stdbool.h>


/* carg_parse() result */
enum carg_status {
    CARG_USERERROR = -2,
    CARG_FATAL = -1,
    CARG_OK = 0,
    CARG_OK_EXIT = 1,
};


/* argument eat result */
enum carg_eatstatus {
    CARG_EAT_OK,
    CARG_EAT_OK_EXIT,
    CARG_EAT_UNRECOGNIZED,
    CARG_EAT_INVALID,
    CARG_EAT_NOTEATEN,
};


/* carg flags */
enum carg_flags{
    CARG_NO_HELP = 1,
    CARG_NO_USAGE = 2,
    CARG_NO_CLOG = 4,
};


struct carg_option;
typedef enum carg_eatstatus (*carg_eater) (const struct carg_option *option,
        const char *value, void *userptr);


/* option flags */
enum carg_optionflags {
    CARG_OPTION_NONE = 0,
};


/* option structure */
struct carg_option {
    const char *name;
    const int key;
    const char *arg;
    enum carg_optionflags flags;
    const char *help;
};


/* Abstract base class! */
struct carg_command {
    const struct carg_option *options;
    const struct carg_subcommand **commands;
    const char *args;
    const char *header;
    const char *footer;
    carg_eater eat;
};


/* Sub Command */
struct carg_subcommand {
    struct carg_command;

    const char *name;
    void *handler;
};


typedef struct carg_state *carg_state_t;
struct carg {
    struct carg_command;

    const char *version;
    enum carg_flags flags;

    /* Internal CArg state */
    carg_state_t state;
};


enum carg_status
carg_parse(struct carg *c, int argc, const char **argv, void *userptr);


#endif  // CARG_H_
