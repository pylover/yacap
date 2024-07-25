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
#include <cutest.h>

#include "carg.h"
#include "helpers.h"


struct rootflags {
    bool foo;
    const char *bar;
};


struct thudflags{
    bool baz;
    const char *arg;
};


static struct rootflags root = {false, NULL};
static struct thudflags thud = {false, NULL};

enum carg_eatstatus
root_eater(const struct carg_option *option, const char *value,
        struct rootflags *flags) {
    if (option == NULL) {
        return CARG_EAT_UNRECOGNIZED;
    }

    switch (option->key) {
        case 'f':
            flags->foo = true;
            break;
        case 'b':
            flags->bar = value;
            break;
        default:
            return CARG_EAT_UNRECOGNIZED;
    }

    return CARG_EAT_OK;
}


enum carg_eatstatus
thud_eater(const struct carg_option *option, const char *value,
        struct thudflags *flags) {
    if (option == NULL) {
        if (flags->arg) {
            return CARG_EAT_UNRECOGNIZED;
        }
        flags->arg = value;
        return CARG_EAT_OK;
    }

    switch (option->key) {
        case 'z':
            flags->baz = true;
            break;
        default:
            return CARG_EAT_UNRECOGNIZED;
    }

    return CARG_EAT_OK;
}


void
test_command() {
    struct carg_option thud_options[] = {
        {"baz", 'z', NULL, 0, "Baz flag"},
        {NULL}
    };

    const struct carg_command thud_cmd = {
        .name = "thud",
        .args = "qux",
        .options = thud_options,
        .eat = (carg_eater_t)thud_eater,
        .userptr = &thud,
    };

    struct carg_option root_options[] = {
        {"foo", 'f', NULL, 0, "Foo flag"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {NULL}
    };

    struct carg carg = {
        .args = NULL,
        .header = NULL,
        .eat = (carg_eater_t)root_eater,
        .options = root_options,
        .footer = NULL,
        .version = NULL,
        .flags = 0,
        .userptr = &root,
        .commands = (const struct carg_command*[]) {
            &thud_cmd,
            NULL
        },
    };

    const struct carg_command *cmd;
    eqint(CARG_OK, carg_parse_string(&carg, "foo thud qux", &cmd));
    isnotnull(cmd);
    eqptr(&thud_cmd, cmd);

    eqint(CARG_OK, carg_parse_string(&carg, "foo", &cmd));
    isnotnull(cmd);
    eqptr(&carg, cmd);

    memset(&root, 0, sizeof(struct rootflags));
    memset(&thud, 0, sizeof(struct thudflags));
    eqint(CARG_OK, carg_parse_string(&carg, "foo -f thud qux", &cmd));
    eqptr(&thud_cmd, cmd);
    istrue(root.foo);

    memset(&root, 0, sizeof(struct rootflags));
    memset(&thud, 0, sizeof(struct thudflags));
    eqint(CARG_OK, carg_parse_string(&carg,
                "foo -f -b qux thud -z quux", &cmd));
    eqptr(&thud_cmd, cmd);
    istrue(root.foo);
    istrue(thud.baz);
    eqstr("qux", root.bar);

    memset(&root, 0, sizeof(struct rootflags));
    memset(&thud, 0, sizeof(struct thudflags));
    eqint(CARG_OK, carg_parse_string(&carg, "foo thud -fzbqux quux", &cmd));
    eqptr(&thud_cmd, cmd);
    istrue(root.foo);
    istrue(thud.baz);
    eqstr("qux", root.bar);
}


int
main() {
    test_command();
    return EXIT_SUCCESS;
}
