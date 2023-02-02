# argl

## Name
Following the logic of the C/C++ argument naming convention.

| name | meaning           |
| ---- | ----------------- |
| argc | argument count    |
| argv | argument vector   |
| argl | argument library  |

## Interface
argl follows the general design principles of [stb](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt). E.G. ARGL_IMPLEMENTATION must be defined before the last inclusion of `argl.h`.

### General
Command line arguments are registered with pointers/references then `argc` and `argv` are parse by argl and the values of the registered arguments are set by pointer. Arguments have types e.g. string, bool, etc. The type is set when registering and they determine what to do when parsing. Boolean arguments are special because they don't take any value after them, having the argument on the command line at all sets the value. Arguments have names and shorthands. Names require the '--' prefix and shorthands require '-'.

### C++
Arguments are registered with the `argl::register_argument(<type>& value, std::string name, std::string shorthand = "")`. The arguments are parsed with `argl::parse_argument`. For example to parse a long with the name `value` and shorthand `v`:
```cpp

#define ARGL_IMPLEMENTATION
#include "argl.h"

int main(int argc, char** argv) {
    long value;
    argl::register_argument(value, "value", "v");

    argl::parse_arguments(argc, argv);
}
```

### C
Arguments are registered with the `argl_register_<type>_argument(<type>& value, char* name)` or `argl_register_<type>_argument_with_shorthand(<type>& value, char* name, char* shorthand)`. The arguments are parsed with `argl_parse_argument`. For example to parse a long with the name `value` and shorthand `v`:
```c

#define ARGL_IMPLEMENTATION
#include "argl.h"

int main(int argc, char** argv) {
    long value;
    argl_register_long_argument_with_shorthand(&value, "value", "v");

    argl_parse_arguments(argc, argv);
}
```
