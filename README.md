# jimmy

a nobuild c build system.

> [!NOTE]
> This currently only works on windows and with clang
>
> linux and macos will come dw i use them too

## usage
to use jimmy, simply compile jimmy.c and run it. that's it. no bs.
```sh
clang jimmy.c -o jimmy.exe
./jimmy.exe

# jimmy, a c build orchestrator

# available commands:
#     build     compile the current project
#     init      create a new project
#     clean     remove the build directory
#     run       run the build
#     vendor    vendor a package via git
#     check     evaluate if jimmy can run properly
```

jimmy is rather opinionated:
- `src/` is the source code directory
- `vendor/` is the directory for third party plugins
- `jimmy_config.h` for changing how jimmy works
- configured as a unity build (main.c specifically)

## why do this?

primarily, it's a learning project

but i also hate the current offerings of build systems
for c. it seems there's three paths:
1. learn a completely different language just to compile
your shit because other build systems will use anything
but c, and some are too unknown to the point that no one
is realistically gonna build your project since they 
don't have it installed. they're also too slow for my liking
2. calling the compiler directly or writing recipes, but 
writing cross-platform Makefiles, batch files and shell 
scripts to get the code to compile on the three main 
platforms have gotten cumbersome just enough for me to 
envy other build systems like zig and cargo (rust).
3. nobuilds! i love this idea, but it seems like the
popular ones (nob.h, nabs) require you to learn their
functions just to compile, which is very much in the
right direction, but i just want sensible defaults

this project is exactly what i want. the build system
itself is written in c, and you can just compile only 
one file to use it. no dependencies, and no need to
write recipes; just bring your compiler. and because 
it's a unity build, compilation is simple and fast 
(for my projects). yes, it's a lot less customizable
than nob.h, but the most you'll need to edit is 
jimmy_config.h, which you'll almost never need to touch 

### note on ai usage
this project has 0% ai-generated code; it simply is 
my exploration into running win32 and posix syscalls 
from c. 

however! although it is a learning project, i use it
quite a lot in my other toy projects.
