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
#include <limits.h>

#include <cutest.h>

#include "arghint.c"


void
test_arghint_validate() {
    eqint(0, arghint_validate(0, arghint_parse(NULL)));
    eqint(-1, arghint_validate(1, arghint_parse(NULL)));
    eqint(-1, arghint_validate(2, arghint_parse(NULL)));

    eqint(0, arghint_validate(0, arghint_parse("")));
    eqint(-1, arghint_validate(1, arghint_parse("")));
    eqint(-1, arghint_validate(2, arghint_parse("")));

    eqint(-1, arghint_validate(0, arghint_parse("FOO")));
    eqint(0, arghint_validate(1, arghint_parse("FOO")));
    eqint(-1, arghint_validate(2, arghint_parse("FOO")));

    eqint(0, arghint_validate(0, arghint_parse("[FOO]")));
    eqint(0, arghint_validate(1, arghint_parse("[FOO]")));
    eqint(-1, arghint_validate(2, arghint_parse("[FOO]")));

    eqint(-1, arghint_validate(0, arghint_parse("FOO BAR")));
    eqint(-1, arghint_validate(1, arghint_parse("FOO BAR")));
    eqint(0, arghint_validate(2, arghint_parse("FOO BAR")));
    eqint(-1, arghint_validate(3, arghint_parse("FOO BAR")));

    eqint(-1, arghint_validate(0, arghint_parse("FOO BAR BAZ")));
    eqint(-1, arghint_validate(1, arghint_parse("FOO BAR BAZ")));
    eqint(-1, arghint_validate(2, arghint_parse("FOO BAR BAZ")));
    eqint(0, arghint_validate(3, arghint_parse("FOO BAR BAZ")));

    eqint(-1, arghint_validate(0, arghint_parse("FOO [BAR]")));
    eqint(0, arghint_validate(1, arghint_parse("FOO [BAR]")));
    eqint(0, arghint_validate(2, arghint_parse("FOO [BAR]")));
    eqint(-1, arghint_validate(3, arghint_parse("FOO [BAR]")));

    eqint(0, arghint_validate(0, arghint_parse("[FOO [BAR]]")));
    eqint(0, arghint_validate(1, arghint_parse("[FOO [BAR]]")));
    eqint(0, arghint_validate(2, arghint_parse("[FOO [BAR]]")));
    eqint(-1, arghint_validate(3, arghint_parse("[FOO [BAR]]")));

    eqint(-1, arghint_validate(0, arghint_parse("FOO [BAR [BAZ]]")));
    eqint(0, arghint_validate(1, arghint_parse("FOO [BAR [BAZ]]")));
    eqint(0, arghint_validate(2, arghint_parse("FOO [BAR [BAZ]]")));
    eqint(0, arghint_validate(3, arghint_parse("FOO [BAR [BAZ]]")));
    eqint(-1, arghint_validate(4, arghint_parse("FOO [BAR [BAZ]]")));

    eqint(-1, arghint_validate(0, arghint_parse("FOO [BAR BAZ]")));
    eqint(0, arghint_validate(1, arghint_parse("FOO [BAR BAZ]")));
    eqint(-1, arghint_validate(2, arghint_parse("FOO [BAR BAZ]")));
    eqint(0, arghint_validate(3, arghint_parse("FOO [BAR BAZ]")));
    eqint(-1, arghint_validate(4, arghint_parse("FOO [BAR BAZ]")));

    eqint(0, arghint_validate(0, arghint_parse("...")));
    eqint(0, arghint_validate(1, arghint_parse("...")));
    eqint(0, arghint_validate(2, arghint_parse("...")));
    eqint(0, arghint_validate(3, arghint_parse("...")));
    eqint(0, arghint_validate(4, arghint_parse("...")));

    eqint(-1, arghint_validate(0, arghint_parse("FOO...")));
    eqint(0, arghint_validate(1, arghint_parse("FOO...")));
    eqint(0, arghint_validate(2, arghint_parse("FOO...")));
    eqint(0, arghint_validate(3, arghint_parse("FOO...")));
    eqint(0, arghint_validate(4, arghint_parse("FOO...")));

    eqint(0, arghint_validate(0, arghint_parse("[FOO]...")));
    eqint(0, arghint_validate(1, arghint_parse("[FOO]...")));
    eqint(0, arghint_validate(2, arghint_parse("[FOO]...")));
    eqint(0, arghint_validate(3, arghint_parse("[FOO]...")));
    eqint(0, arghint_validate(4, arghint_parse("[FOO]...")));

    eqint(-1, arghint_validate(0, arghint_parse("FOO BAR...")));
    eqint(-1, arghint_validate(1, arghint_parse("FOO BAR...")));
    eqint(0, arghint_validate(2, arghint_parse("FOO BAR...")));
    eqint(0, arghint_validate(3, arghint_parse("FOO BAR...")));
    eqint(0, arghint_validate(4, arghint_parse("FOO BAR...")));

    eqint(0, arghint_validate(255, arghint_parse("FOO BAR...")));
}


void
test_arghint_parse() {
    eqint(0b00000000000000000000000000000001, arghint_parse(NULL));
    eqint(0b00000000000000000000000000000001, arghint_parse(""));

    eqint(0b00000000000000000000000000000010, arghint_parse("FOO"));
    eqint(0b00000000000000000000000000000011, arghint_parse("[FOO]"));
    eqint(0b00000000000000000000000000000100, arghint_parse("FOO BAR"));
    eqint(0b00000000000000000000000000001000, arghint_parse("FOO BAR BAZ"));
    eqint(0b00000000000000000000000000000110, arghint_parse("FOO [BAR]"));
    eqint(0b00000000000000000000000000000111, arghint_parse("[FOO [BAR]]"));
    eqint(0b00000000000000000000000000001110, arghint_parse("FOO [BAR [BAZ]]")); // NOLINT
    eqint(0b00000000000000000000000000001010, arghint_parse("FOO [BAR BAZ]"));

    eqint(0b10000000000000000000000000000001, arghint_parse("..."));
    eqint(0b10000000000000000000000000000010, arghint_parse("FOO..."));
    eqint(0b10000000000000000000000000000011, arghint_parse("[FOO]..."));
    eqint(0b10000000000000000000000000000100, arghint_parse("FOO BAR..."));
}


void
test_arghint_parse_errors() {
    eqint(-1, arghint_parse("[FOO...]"));
    eqint(-1, arghint_parse("[...]"));
    eqint(-1, arghint_parse("FOO]"));
    eqint(-1, arghint_parse("[FOO"));
}


int
main() {
    test_arghint_validate();
    test_arghint_parse();
    test_arghint_parse_errors();
    return EXIT_SUCCESS;
}
