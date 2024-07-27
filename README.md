# yacap!
Yet another C argument parser with sub-command support.


## Features
- sub-comman hirerarchy
- positional argument vallidation
- [clog](https://github.com/pylover/clog) integration


## Build & Install 

### From source 
```bash
mkdir build
cmake path/to/source
make menu
make all
make install
```

### Debian package
```bash
cd build
cpack
sudo dpkg -i libyacap-*.deb
```

## Quickstart

```c
#include <stdio.h>
#include <stdlib.h>

#include "yacap.h"


static enum yacap_eatstatus
_eat(const struct yacap_option *opt, const char *value) {
    if (opt == NULL) {
        /* Positional */
        printf("Positional: %s\n", value);
        return YACAP_EAT_OK;
    }

    switch (opt->key) {
        case 'f':
            printf("foo option: --%s=%s\n", opt->name, value);
            break;
        case 'b':
            printf("bar option: --%s=%s\n", opt->name, value);
            break;
        case 'z':
            printf("baz flag: --%s=%s\n", opt->name, value);
            break;
        default:
            return YACAP_EAT_UNRECOGNIZED;
    }

    return YACAP_EAT_OK;
}


/* create and configure a yacap structure */
static struct yacap cli = {
    .eat = (yacap_eater_t)_eat,
    .options = (const struct yacap_option[]) {
        {"foo", 'f', "FOO", 0, "Foo option with value"},
        {"bar", 'b', "BAR", 0, "Bar option with value"},
        {"baz", 'z', NULL, 0, NULL},
        {NULL} // vector termination
    },
};


int
main(int argc, const char **argv) {
    enum yacap_status status = yacap_parse(&cli, argc, argv, NULL);
    yacap_dispose(&cli);

    if (status >= YACAP_OK) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
```

See `examples` directory for other usages such as sub-commands.


## Contribution

### Running all tests
```bash
make test
```

### Running specific test
```bash
cd build
make test_<name>_[exec|debug|profile]
```

For example
```bash
make test_option_exec
make test_option_debug
make test_option_profile
```
