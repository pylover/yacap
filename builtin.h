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
#ifndef BUILTIN_H_
#define BUILTIN_H_

#include "yacap.h"


#ifdef YACAP_USE_CLOG
extern const struct yacap_option opt_verbosity;
extern const struct yacap_option opt_verboseflag;
extern const struct yacap_option opt_quietflag;
#endif
extern const struct yacap_option opt_version;
extern const struct yacap_option opt_help;
extern const struct yacap_option opt_usage;


#endif  // BUILTIN_H_
