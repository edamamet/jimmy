# jimmy

a nobuild c implementation, because build systems suck.

jimmy is compiled using jimmy, if that has any importance 
to you

currently supports `gcc`, `clang`, and even `chibicc` and
`tinycc`, though i haven't tested on macos or windows yet

this project is my simple take on 
[tsoding's nob](https://github.com/tsoding/nob.h)

# usage

to use jimmy, `#define JIMMY_IMPL` before including 
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

compile and run jimmy to compile your program. it's that simple:

```sh
cc jimmy.c -o jimmy && ./jimmy
```
