# jimmy

a nobuild c build system.

> [!NOTE]
> This currently only works on Windows and with clang

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
