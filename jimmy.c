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
