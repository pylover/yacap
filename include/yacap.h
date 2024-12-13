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
#ifndef YACAP_H_
#define YACAP_H_


#include <stdbool.h>


#ifdef __clang__
#define _Nonnull _Nonnull
#define _Nullable _Nullable
#elif defined(__GNUC__)
#define _Nonnull __attribute__((nonnull))
#define _Nullable
#else
#define _Nonnull
#define _Nullable 888
#endif


/* yacap_parse() result */
enum yacap_status {
    YACAP_USERERROR = -2,
    YACAP_FATAL = -1,
    YACAP_OK = 0,
    YACAP_OK_EXIT = 1,
};


/* argument eat result */
enum yacap_eatstatus {
    YACAP_EAT_OK,
    YACAP_EAT_OK_EXIT,
    YACAP_EAT_UNRECOGNIZED,
    YACAP_EAT_INVALID,
    YACAP_EAT_NOTEATEN,
};


/* yacap flags */
enum yacap_flags{
    YACAP_NO_HELP = 1,
    YACAP_NO_USAGE = 2,
    YACAP_NO_CLOG = 4,
};


struct yacap;
struct yacap_option;
struct yacap_command;
typedef enum yacap_eatstatus (*yacap_eater_t)
    (const struct yacap_option *option, const char *value, void *userptr);
typedef int (*yacap_entrypoint_t) (const struct yacap *c,
        const struct yacap_command *cmd);


/* option flags */
enum yacap_optionflags {
    YACAP_OPTION_NONE = 0,
    YACAP_OPTION_MULTIPLE = 1,
};


/* option structure */
struct yacap_option {
    const char *name;
    const int key;
    const char *arg;
    enum yacap_optionflags flags;
    const char *help;
};


/* Abstract base class! */
struct yacap_command {
    const char *name;
    const struct yacap_option * _Nullable options;
    const struct yacap_command ** _Nullable commands;
    const char *args;
    const char *header;
    const char *footer;
    yacap_eater_t eat;
    void *userptr;
    yacap_entrypoint_t entrypoint;
};


typedef struct yacap_state *yacap_state_t;
struct yacap {
    struct yacap_command;

    const char *version;
    enum yacap_flags flags;

    /* Internal yacap state */
    yacap_state_t state;
};


enum yacap_status
yacap_parse(struct yacap *c, int argc, const char **argv,
        const struct yacap_command **command);


int
yacap_dispose(struct yacap *c);


void
yacap_usage_print(const struct yacap *c);


void
yacap_help_print(const struct yacap *c);


int
yacap_try_help(const struct yacap *c);


int
yacap_commandchain_print(int fd, const struct yacap *c);


#endif  // YACAP_H_
