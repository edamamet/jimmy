# jimmy

a nobuild c implementation, because build systems suck.

jimmy is compiled using jimmy, if that has any importance 
to you

# usage

To use jimmy, `#define JIMMY_IMPL` before including 
`jimmy.h`, and simply create a `CompileParams` 
struct, filling it with your compile options:

```c
#define JIMMY_IMPL
#define JIMMY_DEBUG
#include "./jimmy.h"
int main(void) {
    CompileParams params = {
        .Flags = WALL | WERROR | WEXTRA,
        .Source = "jimmy.c",
        .Output = "build"
    };
    Compile(params);
    return 0;
}
```
