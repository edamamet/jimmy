#include <windows.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

void Jimmy_StatPath(const char *path) {
    struct stat stats;
    if (stat(path, &stats) != 0) {
        // return;
    }
    printf(
        "stat result for %s:\n"
        "    S_IFMT   (type of file):       %s\n"
        "    S_IFCHR  (character special):  %s\n"
        "    S_IFREG  (regular):            %s\n"
        "    S_IFDIR  (directory):          %s\n",
        path,
        ((stats.st_mode & S_IFMT) != 0)  ? "ON" : "OFF",
        ((stats.st_mode & S_IFCHR) != 0) ? "ON" : "OFF",
        ((stats.st_mode & S_IFREG) != 0) ? "ON" : "OFF",
        ((stats.st_mode & S_IFDIR) != 0) ? "ON" : "OFF"
    );
}

bool Jimmy_DirExists(const char *path) {
    struct stat stats;
    if (stat(path, &stats) != 0)
        return false;
    return ((stats.st_mode & S_IFDIR) != 0);
}

bool Jimmy_CreateDir(const char *path) {
    return _mkdir(path) == 0;
}

bool Jimmy_FileExists(const char *path) {
    struct stat stats;
    if (stat(path, &stats) != 0)
        return false;
    return ((stats.st_mode & S_IFREG) != 0);
}

bool Jimmy_CreateFile(const char *path, const char *data, size_t dataSize, bool force) {
    if (Jimmy_FileExists(path) && !force)
        return false;
    FILE *mainSrc; 
    fopen_s(&mainSrc, path, "wb");
    if (mainSrc == NULL) {
        printf("Failed to create file at %s, aborting\n", path);
        return false;
    }
    if (data != NULL) {
        fwrite(data, 1, dataSize, mainSrc);
    }
    fclose(mainSrc);
    return true;
}

void Jimmy_StartTimer() {
}

void Build() {
    char path[256];
    uint32_t result = SearchPath(
            NULL,
            "clang",
            ".exe",
            256,
            path,
            NULL
            );
    if (result == 0) {
        printf("uh oh! clang is not in the PATH. it is required to use jimmy\n");
        return;
    }
    Jimmy_CreateDir("build");
    printf("compiling in release mode\n");
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    int errorCode = system("clang src/main.c -o build/main.exe");
    QueryPerformanceCounter(&end);
    double elapsedSeconds = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    if (errorCode != 0) {
        printf("compilation failed (error code %i)\n", errorCode);
        return;
    }
    printf("compiliation finished in %.5f seconds\n", elapsedSeconds);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf(
                "jimmy, a c build orchestrator\n"
                "\n"
                "available commands:\n"
                "    build     compile the current project\n"
                "    init      create a new project\n"
                "    clean     remove the build directory\n"
                "    run       run the build\n"
                "    vendor    vendor a package via git\n"
              );
        return 0;
    }

    if (strcmp(argv[1], "build") == 0) {
        Build();
    } else if (strcmp(argv[1], "init") == 0) {
        if (!Jimmy_CreateDir("src")) {
            printf("project is already initialized, aborting\n");
            return 0;
        }
        const char *sourceCode =
            "#include <stdio.h>\n"
            "\n"
            "int main() {\n"
            "    printf(\"what's up\\n\");\n"
            "}\n";
        if (Jimmy_CreateFile("src/main.c", sourceCode, strlen(sourceCode), false)) 
            printf("project initialized\n");
    } else if (strcmp(argv[1], "clean") == 0) {
        system("rmdir /s /q build");
    } else if (strcmp(argv[1], "run") == 0) {
        if (_access("build", 0) != 0) {
            printf("no build detected (run jimmy build)\n");
            return 0;
        }
        // Build();
        system("build\\main.exe");
    } else if (strcmp(argv[1], "vendor") == 0) {
        printf("vendoring is coming soon!");
    } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0 ) {
        printf("jimmy v0.2beta\n");
    } else if (strcmp(argv[1], "inject") == 0) {
        char exePath[256];
        GetModuleFileName(NULL, exePath, sizeof(exePath));
        char *lastSlash = strrchr(exePath, '\\');
        *lastSlash = '\0';
        char srcPath[280];
        snprintf(srcPath, sizeof(srcPath), "%s\\jimmy.c", exePath);
        CopyFile(srcPath, "jimmy.c", false);
        printf("injected jimmy.c into the current directory\n");
    } else if (strcmp(argv[1], "stat") == 0) {
        Jimmy_StatPath("src/main.c");
        Jimmy_StatPath("src/");
        Jimmy_StatPath("moga/");
    } else {
        printf("HOLLUP! %s is not a command\n", argv[1]);
    }
}
