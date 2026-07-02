# jimmy

a nobuild c build system.

> [!NOTE]
> This currently only works on windows and with clang
>
> linux and macos will come dw i use them too

to use jimmy, simply include jimmy.c in your project and run it. that's it. no bs.
```sh
clang ./jimmy.c -o jimmy.exe
./jimmy.exe

# jimmy, a c build orchestrator

# available commands:
#     build     compile the current project
#     init      create a new project
#     clean     remove the build directory
#     run       run the build
#     vendor    vendor a package via git
```

jimmy is rather opinionated:
- `src/` is the source code directory
- `vendor/` is the directory for third party plugins
- configured as a unity build (main.c specifically)

## why do this?

primarily, it's a learning project

but i also hate the current offerings of build systems
for c. it seems there's three paths. either you have to
learn a completely different language just to compile
your shit, or the build system is so unknown that no
one is realistically gonna build your project since
they don't have it installed. 

they're also slow af

the third is the only
respectable one and that's calling the compiler directly,
but having to write cross-platform Makefiles or writing
different batch files and shell scripts to get the code
to compile on the three main platforms has gotten 
cumbersome just enough for me to envy other build systems
like zig and cargo (rust).

this project is exactly what i want. the build system
itself is written in c, and you can just compile only 
one file to use it. no dependencies; just bring your
compiler. and because it's a unity build, compilation 
is simple and fast (for my projects).

### note on ai usage
this project has 0% ai-generated code; it simply is 
my exploration into running syscalls from c. 

however! although it is a learning project, i use it
quite a lot in my other toy projects.
