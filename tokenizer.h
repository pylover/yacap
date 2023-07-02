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
#ifndef TOKENIZER_H_
#define TOKENIZER_H_


#include "option.h"


struct tokenizer;


struct carg_token {
    const char *value;
    struct carg_option *option;
    int occurance;
};


struct tokenizer *
tokenizer_new(struct carg_option *options, int argc, char **argv);


void
tokenizer_dispose(struct tokenizer *t);


int
tokenizer_next(struct tokenizer *t, struct carg_token *token);


#endif  // TOKENIZER_H_
