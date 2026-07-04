#if __has_include("jimmy_config.h")
#include "jimmy_config.h"
#else
#endif

#ifdef JIMMY_LOG_INFO
#define INFO(s, ...) printf("[INFO] "s, __VA_ARGS__)
#else
static inline void Jimmy_ConsumeFmt(const char *fmt, ...) { (void)fmt; }
#define INFO(s, ...) Jimmy_ConsumeFmt(s, __VA_ARGS__)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define u32 uint32_t
#define u64 uint64_t

extern const char *Platform_DefaultMainSourceCode;

bool Platform_FileExists(const char *path);
void Platform_CreateFile(const char *path);
void Platform_WriteToFile(const char *path, const char *contents);
void Platform_RemoveFile(const char *path);
bool Platform_RenameFile(const char *oldPath, const char *newPath);
u64 Platform_GetFileLastWriteTime(const char *path);
bool Platform_DirExists(const char *path);
void Platform_CreateDir(const char *path);
void Platform_RemoveDir(const char *path);
int Platform_ExecuteShell(const char *cmd);
bool Platform_ShellCommandExists(const char *cmd);
void Platform_ReplaceProcess(const char *binPath, char *cmdline);

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>

const char *Platform_DefaultMainSourceCode =
    "#pragma comment(linker, \"/subsystem:console\")\n"
    "#include <stdio.h>\n"
    "\n"
    "int main() {\n"
    "   printf(\"what's up\");\n"
    "}";

void Win32_StatPath(const char *path) {
    printf("showing path stat for %s\n", path);
    u32 attribs = GetFileAttributes(path);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        printf("HOLLUP! path %s does not exist\n", path);
        return;
    }
    printf("FILE_ATTRIBUTE_READONLY:               %s\n", ((attribs & FILE_ATTRIBUTE_READONLY) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_HIDDEN:                 %s\n", ((attribs & FILE_ATTRIBUTE_HIDDEN) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_SYSTEM:                 %s\n", ((attribs & FILE_ATTRIBUTE_SYSTEM) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_DIRECTORY:              %s\n", ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_ARCHIVE:                %s\n", ((attribs & FILE_ATTRIBUTE_ARCHIVE) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_DEVICE:                 %s\n", ((attribs & FILE_ATTRIBUTE_DEVICE) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_NORMAL:                 %s\n", ((attribs & FILE_ATTRIBUTE_NORMAL) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_TEMPORARY:              %s\n", ((attribs & FILE_ATTRIBUTE_TEMPORARY) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_SPARSE_FILE:            %s\n", ((attribs & FILE_ATTRIBUTE_SPARSE_FILE) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_REPARSE_POINT:          %s\n", ((attribs & FILE_ATTRIBUTE_REPARSE_POINT) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_COMPRESSED:             %s\n", ((attribs & FILE_ATTRIBUTE_COMPRESSED) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_OFFLINE:                %s\n", ((attribs & FILE_ATTRIBUTE_OFFLINE) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_NOT_CONTENT_INDEXED:    %s\n", ((attribs & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_ENCRYPTED:              %s\n", ((attribs & FILE_ATTRIBUTE_ENCRYPTED) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_INTEGRITY_STREAM:       %s\n", ((attribs & FILE_ATTRIBUTE_INTEGRITY_STREAM) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_VIRTUAL:                %s\n", ((attribs & FILE_ATTRIBUTE_VIRTUAL) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_NO_SCRUB_DATA:          %s\n", ((attribs & FILE_ATTRIBUTE_NO_SCRUB_DATA) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_EA:                     %s\n", ((attribs & FILE_ATTRIBUTE_EA) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_PINNED:                 %s\n", ((attribs & FILE_ATTRIBUTE_PINNED) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_UNPINNED:               %s\n", ((attribs & FILE_ATTRIBUTE_UNPINNED) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_RECALL_ON_OPEN:         %s\n", ((attribs & FILE_ATTRIBUTE_RECALL_ON_OPEN) != 0 ? "ON":"OFF"));
    printf("FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS:  %s\n", ((attribs & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) != 0 ? "ON":"OFF"));
}

bool Platform_DirExists(const char *path) {
    u32 attribs = GetFileAttributes(path);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        INFO("dir %s does not exist\n", path);
        return false;
    }
    INFO("dir %s exists\n", path);
    return ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0);
}
void Platform_CreateDir(const char *path) {
    if (_mkdir(path) == 0)
        INFO("created directory %s/\n", path);
} void Platform_RemoveDir(const char *path) {
    _rmdir(path);
    INFO("removed directory %s/\n", path);
}
bool Platform_FileExists(const char *path) {
    u32 attribs = GetFileAttributes(path);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        INFO("file %s does not exist\n", path);
        return false;
    }
    INFO("file %s exists\n", path);
    return ((attribs & FILE_ATTRIBUTE_DIRECTORY) == 0);
}
void Platform_CreateFile(const char *path) {
    FILE *file;
    fopen_s(&file, path, "a");
    fclose(file);
    INFO("created file %s/\n", path);
}
void Platform_WriteToFile(const char *path, const char *contents) {
    FILE *file;
    fopen_s(&file, path, "wb");
    int bytesWritten = fprintf(file, "%s", contents);
    fclose(file);
    INFO("wrote %i bytes to %s/\n", bytesWritten, path);
}
void Platform_RemoveFile(const char *path) {
    bool success = DeleteFile(path);
    if (success)
        INFO("deleted file %s/\n", path);
}
bool Platform_RenameFile(const char *oldPath, const char *newPath) {
    return MoveFile(oldPath, newPath);
}
u64 Platform_GetFileLastWriteTime(const char *path) {
    WIN32_FILE_ATTRIBUTE_DATA attribData;
    if (!GetFileAttributesEx(path, GetFileExInfoStandard, &attribData)) {
        return 0;
    }
    ULARGE_INTEGER writeTime = {
        .LowPart = attribData.ftLastWriteTime.dwLowDateTime,
        .HighPart = attribData.ftLastWriteTime.dwHighDateTime,
    };
    return writeTime.QuadPart;
}
int Platform_ExecuteShell(const char *cmd) {
    char cmdline[256] = {};
    int len = snprintf(cmdline, sizeof(cmdline), "%s", cmd);
    for (int i = 0; i < len; i++) {
        if (cmdline[i] == '/') cmdline[i] = '\\';
    }
    return system(cmdline);
}
bool Platform_ShellCommandExists(const char *cmd) {
    char executableName[128];
    u32 returnLen = SearchPath(NULL, cmd, ".exe", sizeof(executableName), executableName, NULL);
    if (returnLen == 0)
        return false;
    // printf("%s\n", executableName);
    return true;
}
void Platform_ReplaceProcess(const char *binPath, char *cmdline) {
    STARTUPINFO startupInfo = {.cb = sizeof(startupInfo)};
    PROCESS_INFORMATION processInfo = {};
    if (!CreateProcess(binPath, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) {
        printf("failed to create process, aborting\n");
        exit(1);
    }
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    DWORD code;
    GetExitCodeProcess(processInfo.hProcess, &code);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    exit(code);
}
#else
#endif

const char *Jimmy_DefaultConfigCode = 
"#define JIMMY_LOG_INFO      // enables [Info] logs (there's quite a lot of them!)\n"
"\n" 
"#define JIMMY_AUTO_REBUILD  // auto-rebuild jimmy on detected changes\n"
"                            //     occassionally faster start-up times,\n"  
"                            //     but you'll need to manually recompile\n"
"                            //     jimmy when editing the project (including\n"
"                            //     this config file!)";

void Jimmy_RebuildSelf(int argc, char **argv) {
    char *thisBinPath = argv[0];
    INFO("checking rebuild conditions...\n");
    char oldBinPath[256];
    snprintf(oldBinPath, sizeof(oldBinPath), "%s.old.exe", thisBinPath);
    if (Platform_FileExists(oldBinPath)) {
        Platform_RemoveFile(oldBinPath);
    }
    INFO("querying project times against self (%s)\n", thisBinPath);

    u64 jimmyTime = Platform_GetFileLastWriteTime(thisBinPath);
    if (jimmyTime == 0) {
        // printf("could not query self (%s), aborting\n", thisBinPath);
        printf("welcome back?\n");
        return;
    }
    u64 configTime = Platform_GetFileLastWriteTime("jimmy_config.h");
    u64 mainTime = Platform_GetFileLastWriteTime("src/");

    INFO("last build times:\n");
    INFO("    jimmy      %zu\n", jimmyTime);
    INFO("    config.h   %zu\n", configTime);
    INFO("    src/       %zu\n", mainTime);

    if (jimmyTime > configTime && jimmyTime > mainTime) {
        INFO("no need to rebuild :)\n");
        return;
    }

    INFO("REBUILDING...\n");
    if (!Platform_RenameFile(thisBinPath, oldBinPath)) {
        printf("failed to rename %s to %s, aborting\n", thisBinPath, oldBinPath);
        return;
    }
    char command[1024];
    snprintf(command, sizeof(command), "clang -o %s %s -Werror -Wall -Wextra", thisBinPath, __FILE__);
    int errorCode = Platform_ExecuteShell(command);
    if (errorCode != 0) {
        if (!Platform_RenameFile(oldBinPath, thisBinPath)) {
            printf("failed to rename %s to %s, aborting\n", thisBinPath, oldBinPath);
        }
        printf("rebuild failed, aborting\n");
        exit(1);
    }

    char cmdline[512];
    size_t pos = 0;
    for (int i = 1; i < argc; i++) {
        int len = snprintf(cmdline + pos, sizeof(cmdline) - pos, " \"%s\"", argv[i]);
        pos += len;
    }
    Platform_ReplaceProcess(thisBinPath, cmdline);
}

void Jimmy_Build() { 
    if (!Platform_FileExists("src/main.c")) {
        printf("src/main.c does not exist, aborting\n");
        return;
    }
    Platform_CreateDir("build");
    int errorCode = Platform_ExecuteShell("clang -o build/main.exe src/main.c -Werror -Wall -Wextra");
    if (errorCode == 0) {
        printf("compilation complete\n");
    } else {
        printf("compilation failed\n");
    }
}
void Jimmy_Init() { 
    if (Platform_DirExists("src")) {
        if (!Platform_FileExists("jimmy_config.h")) {
            printf("restoring jimmy_config.h\n");
            Platform_CreateFile("jimmy_config.h");
            Platform_WriteToFile("jimmy_config.h", Jimmy_DefaultConfigCode);
        }
        printf("project already initialized, aborting\n");
        return;
    } 
    Platform_CreateDir("src");
    Platform_CreateFile("src/main.c");
    Platform_WriteToFile("src/main.c", Platform_DefaultMainSourceCode);
    Platform_CreateFile("jimmy_config.h");
    Platform_WriteToFile("jimmy_config.h", Jimmy_DefaultConfigCode);
    printf("project initialized");
}
void Jimmy_Clean() { 
    Platform_RemoveDir("build");
}
void Jimmy_Run() { 
    Platform_ExecuteShell("build/main.exe");
}
void Jimmy_Vendor() { 
    printf("vendoring coming soon(tm)\n");
}
void Jimmy_Check() { 
    printf("checking shell capabilities...\n");
    bool clangAvail = Platform_ShellCommandExists("clang");
    bool tinyccAvail = Platform_ShellCommandExists("tcc");
    bool cmakeAvail = Platform_ShellCommandExists("cmake");
    bool makeAvail = Platform_ShellCommandExists("make");
    printf(
        "    clang:   %s\n"
        "    tinycc:  %s\n"
        "    cmake:   %s\n"
        "    make:    %s\n",
        clangAvail ? "YES" : "NO", 
        tinyccAvail ? "YES" : "NO", 
        cmakeAvail ? "YES" : "NO", 
        makeAvail ? "YES" : "NO"
    );
}

void PrintHelp() {
    printf(
        "jimmy, a c build orchestrator\n"
        "\n"
        "available commands:\n"
        "    build     compile the current project\n"
        "    init      create a new project\n"
        "    clean     remove the build directory\n"
        "    run       run the build\n"
        "    vendor    vendor a package via git\n"
        "    check     evaluate if jimmy can properly\n"
    );
}

int main(int argc, char** argv) {
    Jimmy_RebuildSelf(argc, argv);
    if (argc == 1) {
        PrintHelp();
        return 0;
    }

    const char *command = argv[1];
    if (strcmp(command, "build") == 0) {
        Jimmy_Build();
    } else if (strcmp(command, "init") == 0) {
        Jimmy_Init();
    } else if (strcmp(command, "clean") == 0) {
        Jimmy_Clean();
    } else if (strcmp(command, "run") == 0) {
        Jimmy_Run();
    } else if (strcmp(command, "vendor") == 0) {
        Jimmy_Vendor();
    } else if (strcmp(command, "check") == 0) {
        Jimmy_Check();
    } else if (strcmp(command, "--version") == 0) {
        printf("jimmy v0.3beta");
    } else {
        printf("unknown command %s\n", command);
    }
}
