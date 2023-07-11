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


/* carg special keys */
#define KEY_ARG -1
#define KEY_END -2


/* carg_parse() result */
enum carg_status {
    CARG_ERR = -1,
    CARG_OK = 0,
    CARG_OK_EXIT = 1,
};


/* argument eat result */
enum carg_eatstatus {
    CARG_EAT_OK,
    CARG_EAT_OK_EXIT,
    CARG_EAT_FLAG,
    CARG_EAT_UNRECOGNIZED,
    CARG_EAT_VALUE_REQUIRED,
    CARG_EAT_BAD_VALUE,
    CARG_EAT_ARG_REQUIRED,
};


/* carg flags */
enum carg_flags{
    CARG_NO_HELP = 2,
    CARG_NO_USAGE = 4,
    CARG_NO_CLOG = 8,
};


struct carg_state;
typedef enum carg_eatstatus (*carg_eater) (int key, const char *value,
        struct carg_state *state);


/* option flags */
enum carg_optionflags {
    CARG_OPTION = 0,
    CARG_OPTIONAL_VALUE = 1,
    CARG_COMMAND = 2,
};


struct carg_option {
    const char *name;
    const int key;
    const char *arg;
    enum carg_optionflags flags;
    const char *help;
};


/* Sub Command */
struct carg_command {
    const char *command;
    carg_eater eat;
    const char *args;
    const struct carg_option *options;
    void *handler;
};


struct carg {
    carg_eater eat;
    const struct carg_option *options;
    const char *args;
    const char *header;
    const char *footer;
    const char *version;
    enum carg_flags flags;
    void *handler;
    const struct carg_command **commands;
};


struct carg_state {
    const struct carg *carg;
    int fd;
    bool dashdash;

    int argc;
    const char **argv;
    void *userptr;
    int index;
    const char *next;
    int arg_index;
    bool last;
};


void
carg_outfile_set(int fd);


void
carg_errfile_set(int fd);


enum carg_status
carg_parse(const struct carg *c, int argc, const char **argv, void *userptr,
        void **handler);


#endif  // CARG_H_
