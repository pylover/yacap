#include <stdlib.h>

#include "option.h"


int
option_arg_parse(const char *arghint) {
    if (arghint == NULL) {
        return CARG_OPTIONARG_NONE;
    }

    return CARG_OPTIONARG_MANDATORY;
}
