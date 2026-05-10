#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <libgen.h>
#include <sys/stat.h>

/*
 *  Usage:
 *
 *  #define JIMMY_IMPL in the build.c to use jimmy
 *  #define JIMMY_DEBUG to enable debug logs
 *  #define JIMMY_NO_PRINT to disable regular logs
 */

#ifdef JIMMY_DEBUG
#define DEBUG(...) printf("[DEBUG] " __VA_ARGS__)
#else
#define DEBUG(...)
#endif

#ifndef JIMMY_NO_PRINT
#define PRINT(...) printf("[INFO]  " __VA_ARGS__)
#else
#define PRINT(...)
#endif

#define ERROR(...) fprintf(stderr, "[ERROR] " __VA_ARGS__)

// Warning Flags
// Common
#define WERROR       (1 << 0)
#define WALL         (1 << 1)
#define WPEDANTIC    (1 << 2)
#define WEXTRA       (1 << 3)
#define WCONVERSION  (1 << 4)

// Clang-Specific
#define WEVERYTHING  (1 << 5)

typedef struct {
    const char *PreferredCompiler;
    const char *Source;
    const char *Output;
    uint16_t Flags;
    bool EnableASAN;    // -fsanitize=address
} CompileParams;

bool CommandExists(const char *command);
const char *GetAvailableCompiler(void);
int CompileRaw(const char *compiler, const char *source, const char *output, const char *flags);
bool ValidateCompileParams(CompileParams *params);
int Compile(CompileParams params);

// #define JIMMY_IMPL
#ifdef JIMMY_IMPL
bool CommandExists(const char *command) {
    char *path = getenv("PATH");
    char *path_copy = strdup(path);
    char *dir = strtok(path_copy, ":");
    while (dir != NULL) { 
        char fullPath[512];
        snprintf(fullPath, 512, "%s/%s", dir, command);
        if (access(fullPath, R_OK) == 0) {
            DEBUG("%s found at %s\n", command, fullPath);
            free(path_copy);
            return true;
        }

        dir = strtok(NULL, ":");
    }
    DEBUG("Could not find command \'%s\' in PATH\n", command);
    free(path_copy);
    return false;
}

const char *GetAvailableCompiler(void) {
    if (CommandExists("gcc")) {
        return "gcc";
    }
    if (CommandExists("clang")) {
        return "clang";
    }
    if (CommandExists("cl")) {
        return "cl";
    }
    if (CommandExists("chibicc")) {
        return "chibicc";
    }
    if (CommandExists("tinycc")) {
        return "tinycc";
    }
    return NULL;
}


int CompileRaw(const char *compiler, const char *source, const char *output, const char *flags) {
    char runPath[256];
    int len = readlink("/proc/self/exe", runPath, sizeof(runPath));
    runPath[len] = '\0';

    char *dirName = dirname(runPath);
    char dirPath[256];
    strncpy(dirPath, dirName, sizeof(dirPath));

    char outputFilePath[512];
    sprintf(outputFilePath, "%s/%s", dirPath, output);
    char outputFilePathClone[512];
    strncpy(outputFilePathClone, outputFilePath, sizeof(outputFilePathClone));
    char *outputDirName = dirname(outputFilePathClone);
    char outputPath[512];
    strncpy(outputPath, outputDirName, sizeof(outputPath));

    int mkdirCode = mkdir(outputPath, 0777);
    if (mkdirCode == -1 && errno != EEXIST) {
        ERROR("Failed to create directory %s\n", dirPath);
        return -1;
    }

    char command[1024];
    snprintf(
        command, 
        sizeof(command), 
        "%s -o %s %s/%s %s",
        compiler,
        outputFilePath,
        dirPath,
        source,
        flags
    );
    DEBUG("Compile command: %s\n", command);
    int errorCode = system(command);
    return errorCode;
}


bool ValidateCompileParams(CompileParams *params) {
    if (params->PreferredCompiler == NULL) {
        DEBUG("Attempting to find a C compiler...\n");
        params->PreferredCompiler = GetAvailableCompiler();
        if (params->PreferredCompiler == NULL) {
            ERROR("You do not have a C compiler. Please install one.\n");
            return false;
        }
    } else {
        if (!CommandExists(params->PreferredCompiler)) {
            // ERROR("%s is not a supported compiler.\n", params->PreferredCompiler);
            // return false;
            params->PreferredCompiler = NULL;
            return ValidateCompileParams(params);
        }
    }

    if (params->Source == NULL) {
        ERROR("Source is missing. Please set CompileParams.Output to the source file.\n");
        return false;
    }

    if (params->Output == NULL) {
        ERROR("Output is missing. Please set CompileParams.Output to the intended output name.\n");
        return false;
    }
    return true;
}

int Compile(CompileParams params) {
    if (!ValidateCompileParams(&params)) {
        return 1;
    }
    DEBUG("Using the %s compiler\n", params.PreferredCompiler);

    char flags[64] = {0};
    size_t flagsWriteIdx = 0;


    if ((params.Flags & WERROR) != 0) {
        const char *flag = "-Werror ";
        strncpy(flags + flagsWriteIdx, flag, strlen(flag));
        flagsWriteIdx += strlen(flag);
    }

    if ((params.Flags & WALL) != 0) {
        const char *flag = "-Wall ";
        strncpy(flags + flagsWriteIdx, flag, strlen(flag));
        flagsWriteIdx += strlen(flag);
    }

    if ((params.Flags & WPEDANTIC) != 0) {
        const char *flag = "-Wpedantic ";
        strncpy(flags + flagsWriteIdx, flag, strlen(flag));
        flagsWriteIdx += strlen(flag);
    }

    if ((params.Flags & WEXTRA) != 0) {
        const char *flag = "-Wextra ";
        strncpy(flags + flagsWriteIdx, flag, strlen(flag));
        flagsWriteIdx += strlen(flag);
    }

    if ((params.Flags & WCONVERSION) != 0) {
        const char *flag = "-Wconversion ";
        strncpy(flags + flagsWriteIdx, flag, strlen(flag));
        flagsWriteIdx += strlen(flag);
    }

    if ((params.Flags & WEVERYTHING) != 0) {
        if (strcmp(params.PreferredCompiler, "clang") != 0) {
            PRINT("Skipping flag -Weverything: %s does not support it\n", params.PreferredCompiler);
        } else {
            const char *flag = "-Weverything ";
            strncpy(flags + flagsWriteIdx, flag, strlen(flag));
            flagsWriteIdx += strlen(flag);
        }
    }

    if (params.EnableASAN) {
        const char *flag = "-fsanitize=address ";
        strncpy(flags + flagsWriteIdx, flag, strlen(flag));
        flagsWriteIdx += strlen(flag);
    }

    int errorCode = CompileRaw(params.PreferredCompiler, params.Source, params.Output, flags);
    if (errorCode == 0) {
        PRINT("Compilation succeeded\n");
    } else {
        PRINT("Compilation failed with error code %i\n", errorCode);
    }
    return 0;
}
#endif
