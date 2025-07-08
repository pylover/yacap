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
#include <cutest.h>

#include "include/yacap.h"
#include "helpers.h"


struct opts {
    bool foo;
    bool bar;
};



static struct opts root = {false, false};
static struct opts add = {false, false};


static enum yacap_eatstatus
_eater(const struct yacap_option *option, const char *value,
        struct opts *opts) {
    if (option == NULL) {
        return YACAP_EAT_UNRECOGNIZED;
    }

    switch (option->key) {
        case 'f':
        case 'F':
            opts->foo = true;
            break;
        case 'b':
        case 'B':
            opts->bar = true;
            break;
        default:
            return YACAP_EAT_UNRECOGNIZED;
    }

    return YACAP_EAT_OK;
}


static struct yacap_option root_options[] = {
    {"rootfoo", 'f', NULL, 0, "Foo flag"},
    {"rootbar", 'b', NULL, 0, "Bar flag"},
    {NULL}
};


static struct yacap_option add_options[] = {
    {"addfoo", 'F', NULL, 0, "Foo flag"},
    {"addbar", 'B', NULL, 0, "Bar flag"},
    {NULL}
};


static struct yacap_command add_cmd = {
    .name = "add",
    .args = "[qux]",
    .options = add_options,
    .eat = (yacap_eater_t)_eater,
    .userptr = &add,
};



static struct yacap yacap = {
    .args = NULL,
    .header = NULL,
    .eat = (yacap_eater_t)_eater,
    .options = root_options,
    .footer = NULL,
    .version = NULL,
    .flags = 0,
    .userptr = &root,
    .commands = (struct yacap_command *const[]) {
        &add_cmd,
        NULL
    },
};


void
test_comand_optionorder() {
    const struct yacap_command *cmd;

    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo", &cmd));
    isnotnull(cmd);
    eqptr(&yacap, cmd);

    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo add", &cmd));
    isnotnull(cmd);
    eqptr(&add_cmd, cmd);

    memset(&root, 0, sizeof(struct opts));
    memset(&add, 0, sizeof(struct opts));
    eqint(YACAP_OK, yacap_parse_string(&yacap, "foo add -f", &cmd));
    istrue(root.foo);
    isfalse(root.bar);
    isfalse(add.foo);
    isfalse(add.bar);

    memset(&root, 0, sizeof(struct opts));
    memset(&add, 0, sizeof(struct opts));
    eqint(YACAP_USERERROR, yacap_parse_string(&yacap, "foo -F add", &cmd));
    eqstr("foo: invalid option -- '-F'\n"
        "Try `foo --help' or `foo --usage' for more information.\n", err);
    isfalse(root.foo);
    isfalse(root.bar);
    isfalse(add.foo);
    isfalse(add.bar);
}


int
main() {
    test_comand_optionorder();
    return EXIT_SUCCESS;
}
