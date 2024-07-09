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
#include <malloc.h>

#include <clog.h>
#include <cutest.h>

#include "optiondb.c"


void
test_optiondb_duplication() {
    struct carg_optiondb optdb;
    optiondb_init(&optdb);

    struct carg_option options1[] = {
        {"aoo", 'a', NULL, 0, NULL},
        {"aoo", 'a', NULL, 0, NULL},
        {NULL}
    };
    eqint(-1, optiondb_insert(&optdb, options1));
    optiondb_dispose(&optdb);

    optiondb_init(&optdb);
    struct carg_option options2[] = {
        {"aoo", 'a', NULL, 0, NULL},
        {"boo", 'b', NULL, 0, NULL},
        {NULL}
    };
    struct carg_option options3[] = {
        {"aoo", 'a', NULL, 0, NULL},
        {NULL}
    };
    eqint(0, optiondb_insert(&optdb, options2));
    eqint(-1, optiondb_insert(&optdb, options3));
    optiondb_dispose(&optdb);
}


void
test_optiondb_autoextend() {
    struct carg_optiondb optdb;
    struct carg_option options1[] = {
        {"aoo", 'a', NULL, 0, NULL},
        {"boo", 'b', NULL, 0, NULL},
        {"coo", 'c', NULL, 0, NULL},
        {"doo", 'd', NULL, 0, NULL},
        {"eoo", 'e', NULL, 0, NULL},
        {NULL}
    };

    struct carg_option options2[] = {
        {"foo", 'f', NULL, 1, NULL},
        {"goo", 'g', NULL, 0, NULL},
        {"hoo", 'h', NULL, 0, NULL},
        {"ioo", 'i', NULL, 0, NULL},
        {"joo", 'j', NULL, 0, NULL},
        {"koo", 'k', NULL, 0, NULL},
        {"loo", 'l', NULL, 0, NULL},
        {"moo", 'm', NULL, 0, NULL},
        {"noo", 'n', NULL, 0, NULL},
        {"ooo", 'o', NULL, 0, NULL},
        {"poo", 'p', NULL, 0, NULL},
        {NULL}
    };


    optiondb_init(&optdb);
    optiondb_insert(&optdb, options1);

    eqint(8, optdb.size);
    eqint(5, optdb.count);
    istrue(sizeof(struct carg_option*) * 8 <=
            malloc_usable_size(optdb.repo));

    optiondb_insert(&optdb, options2);
    eqint(16, optdb.size);
    eqint(16, optdb.count);
    istrue(sizeof(struct carg_option*) * 16 <=
            malloc_usable_size(optdb.repo));
}


int
main() {
    test_optiondb_autoextend();
    test_optiondb_duplication();
    return EXIT_SUCCESS;
}
