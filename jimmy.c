#if __has_include("jimmy_config.h")
#include "jimmy_config.h"
#else
#endif

#define JIMMY_LOG_INFO
#ifdef JIMMY_LOG_INFO
#define INFO(s, ...) printf("[INFO] "s, ##__VA_ARGS__)
#else
static inline void Jimmy_ConsumeFmt(const char *fmt, ...) { (void)fmt; }
#define INFO(s, ...) Jimmy_ConsumeFmt(s, ##__VA_ARGS__)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define u32 uint32_t
#define u64 uint64_t

extern const char *Platform_DefaultMainSourceCode;

void Platform_InitConsole();
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
void Platform_FormatDirString(char *path, size_t size);
bool Platform_ShellCommandExists(const char *cmd);
void Platform_ReplaceProcess(const char *binPath, int argc, char **argv);

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")
void Platform_InitConsole(void) {
    SetConsoleOutputCP(CP_UTF8);
}
const char *Platform_DefaultMainSourceCode =
    "#pragma comment(linker, \"/subsystem:console\")\n"
    "#include <stdio.h>\n"
    "\n"
    "int main() {\n"
    "   printf(\"what's up\\n\");\n"
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
} 
void Platform_RemoveDir(const char *path) {
    char doubleTermPath[256];
    size_t len = snprintf(doubleTermPath, sizeof(doubleTermPath), "%s", path);
    doubleTermPath[len] = '\0';
    doubleTermPath[len+1] = '\0';
    SHFILEOPSTRUCT fileOp = {
        .wFunc = FO_DELETE,
        .pFrom = doubleTermPath,
        .fFlags = FOF_NO_UI,
    };
    int success = SHFileOperation(&fileOp);
    if (success != 0) {
        if (success != 2)
            INFO("failed to remove directory %s/: 0x%02X\n", doubleTermPath, success);
        return;
    }
    INFO("removed directory %s/\n", doubleTermPath);
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
void Platform_FormatDirString(char *path, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (path[i] == '/') path[i] = '\\';
    }
}
int Platform_ExecuteShell(const char *cmd) {
    return system(cmd);
}
bool Platform_ShellCommandExists(const char *cmd) {
    char executableName[128];
    u32 returnLen = SearchPath(NULL, cmd, ".exe", sizeof(executableName), executableName, NULL);
    if (returnLen == 0)
        return false;
    // printf("%s\n", executableName);
    return true;
}
void Platform_ReplaceProcess(const char *binPath, int argc, char **argv) {
    char cmdline[512];
    size_t pos = snprintf(cmdline, sizeof(cmdline), "\"%s\"", binPath);
    for (int i = 1; i < argc; i++) {
        int len = snprintf(cmdline + pos, sizeof(cmdline) - pos, " \"%s\"", argv[i]);
        pos += len;
    }

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
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ftw.h>

const char *Platform_DefaultMainSourceCode =
    "#include <stdio.h>\n"
    "\n"
    "int main() {\n"
    "   printf(\"what's up\\n\");\n"
    "}";

void Platform_InitConsole() {}
bool Platform_DirExists(const char *path) {
    struct stat s;
    bool exists = stat(path, &s);
    if (exists == false)
        return false;
    return (s.st_mode & S_IFDIR) != 0;
}
void Platform_CreateDir(const char *path) {
    if (mkdir(path, 0755) == 0)
        INFO("created directory %s/\n", path);
} 

int POSIX_NftwIterationRemove(const char *path, const struct stat *stat, int flag, struct FTW *ftw) {
    (void)stat;
    (void)flag;
    (void)ftw;
    if (remove(path) != 0) {
        INFO("failed to remove %s\n", path);
        return 1;
    }
    INFO("removed %s\n", path);
    return 0;
}

void Platform_RemoveDir(const char *path) {
    if (nftw(path, POSIX_NftwIterationRemove, 20, FTW_DEPTH | FTW_PHYS) == 0)
        INFO("removed directory %s/\n", path);
}
bool Platform_FileExists(const char *path) {
    struct stat s;
    bool exists = stat(path, &s) == 0;
    if (exists == false) {
        INFO("file %s does not exist\n", path);
        return false;
    }
    INFO("file %s exists\n", path);
    return (s.st_mode & S_IFREG) != 0;
}
void Platform_CreateFile(const char *path) {
    FILE *file = fopen(path, "a");
    fclose(file);
    INFO("created file %s/\n", path);
}
void Platform_WriteToFile(const char *path, const char *contents) {
    FILE *file = fopen(path, "wb");
    int bytesWritten = fprintf(file, "%s", contents);
    fclose(file);
    INFO("wrote %i bytes to %s/\n", bytesWritten, path);
}
void Platform_RemoveFile(const char *path) {
    bool success = remove(path);
    if (success)
        INFO("deleted file %s/\n", path);
}
bool Platform_RenameFile(const char *oldPath, const char *newPath) {
    return rename(oldPath, newPath) == 0;
}
u64 Platform_GetFileLastWriteTime(const char *path) {
    struct stat s;
    bool exists = stat(path, &s) == 0;
    if (exists == false)
        return 0;
    return (u64)s.st_mtime;
}
void Platform_FormatDirString(char *path, size_t size) {
    (void) path;
    (void) size;
}
int Platform_ExecuteShell(const char *cmd) {
    return system(cmd);
}
bool Platform_ShellCommandExists(const char *cmd) {
    (void)cmd;
    const char *envPath = getenv("PATH");
    if (envPath == NULL) {
        INFO("Could not find PATH environment variable");
        return false;
    }
    char buffer[4097];
    snprintf(buffer, sizeof(buffer), "%s", envPath);
    char *currDir = strtok(buffer, ":");
    for(;;) {
        char currPath[1024];
        snprintf(currPath, sizeof(currPath), "%s/%s", currDir, cmd);
        INFO("checking %s...\n", currPath);
        if (access(currPath, F_OK) == 0) {
            INFO("found! %s...\n", currPath);
            return true;
        }
        currDir = strtok(NULL, ":");
        if (currDir == NULL)
            break;
    }
    return false;
}
void Platform_ReplaceProcess(const char *binPath, int argc, char **argv) {
    (void) argc;
    if (execv(binPath, argv) == -1)  {
         printf("failed to create process, aborting\n");
         exit(1);
    }
}
#endif

const char *Jimmy_DefaultConfigCode = 
"// #define JIMMY_LOG_INFO         // enables [Info] logs (there's quite a lot of them!)\n"
"\n"
"// #define JIMMY_NO_AUTO_REBUILD  // disables auto rebuilding\n"
"                                  //     faster startup times, but you'll\n"
"                                  //     have to manually recompile jimmy \n"
"                                  //     to apply config/project changes";

void Jimmy_RebuildSelf(int argc, char **argv) {
    INFO("=== Jimmy_RebuildSelf ===\n");
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
        printf("could not query self (%s), aborting rebuild process\n", thisBinPath);
        return;
    }
    u64 configTime = Platform_GetFileLastWriteTime("jimmy_config.h");
    u64 mainTime = Platform_GetFileLastWriteTime("src/");

    INFO("last build times:\n");
    INFO("    jimmy      %llu\n", jimmyTime);
    INFO("    config.h   %llu\n", configTime);
    INFO("    src/       %llu\n", mainTime);

    if (jimmyTime > configTime && jimmyTime > mainTime) {
        INFO("no need to rebuild :)\n");
        return;
    }

    printf("changes to project detected, rebuilding self...\n");
    if (!Platform_RenameFile(thisBinPath, oldBinPath)) {
        printf("failed to rename %s to %s, aborting rebuild process\n", thisBinPath, oldBinPath);
        return;
    }
    char command[1024];
    snprintf(command, sizeof(command), "clang -o %s %s -Werror -Wall -Wextra", thisBinPath, __FILE__);
    int errorCode = Platform_ExecuteShell(command);
    if (errorCode != 0) {
        if (!Platform_RenameFile(oldBinPath, thisBinPath)) {
            printf("failed to rename %s to %s, aborting rebuild process\n", thisBinPath, oldBinPath);
        }
        printf("rebuild failed, aborting\n");
        exit(1);
    }

    printf("rebuild succeeded, relaunching...\n");
    Platform_ReplaceProcess(thisBinPath, argc, argv);
}

void Jimmy_Build() { 
    INFO("=== Jimmy_Build ===\n");
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
        exit(errorCode);
    }
}
void Jimmy_Init() { 
    INFO("=== Jimmy_Init ===\n");
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
    printf(
        "project initialized\n"
        "├── src/\n"
        "│   └── main.c\n"
        "└── jimmy_config.h\n"
    );
}
void Jimmy_Clean() { 
    INFO("=== Jimmy_Clean ===\n");
    Platform_RemoveDir("build");
}
void Jimmy_Run() { 
    INFO("=== Jimmy_Run ===\n");
    if (!Platform_DirExists("src")) {
        Jimmy_Build();
    }
    u64 srcTime = Platform_GetFileLastWriteTime("src/");
    u64 buildTime = Platform_GetFileLastWriteTime("build/main.exe");
    INFO("comparing edit times:\n");
    INFO("    src/             %llu\n", srcTime);
    INFO("    build/main.exe   %llu\n", buildTime);
    if (srcTime > buildTime) {
        INFO("auto compilation triggered\n");
        Jimmy_Build();
    }
    Platform_ExecuteShell("build/main.exe");
}
void Jimmy_Vendor() { 
    INFO("=== Jimmy_Vendor ===\n");
    printf("vendoring coming soon(tm)\n");
}
void Jimmy_Check() { 
    INFO("=== Jimmy_Check ===\n");
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
        "    check     evaluate if jimmy can run properly\n"
    );
}

int main(int argc, char** argv) {
    Platform_InitConsole();
#ifndef JIMMY_NO_AUTO_REBUILD
    Jimmy_RebuildSelf(argc, argv);
#endif
    if (argc == 1 || strcmp(argv[1], "--help") == 0) {
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
        printf("jimmy v0.4beta");
    } else {
        printf("unknown command %s\n", command);
    }
}
