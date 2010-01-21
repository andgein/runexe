#define _UNICODE

#include <wchar.h>

#include <cstdio>
#include <stdint.h>
#include <cstring>
#include <stdbool.h>
#include <memory>
#include <string>
#include <cstdarg>
#include <iostream>
#include <vector>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <cstdlib>

#include "w32invoke.h"

#define USED

using namespace std;

const static int MAX_FORMAT_LENGTH = 8192;

static vector<string> tokenizeCommandLine(const string& commandLine)
{
    vector<string> result;
    bool inQuotes = false;
    string buffer;
    for (size_t i = 0; i < commandLine.length(); i++)
    {
        if (commandLine[i] == '\"')
        {
            inQuotes = !inQuotes;
            continue;
        }
        if (!inQuotes && commandLine[i] <= ' ')
        {
            if (!buffer.empty() || (i > 0 && commandLine[i - 1] == '\"'))
            {
                result.push_back(buffer);
                buffer = "";
            }
            continue;
        }
        buffer.append(1, commandLine[i]);
    }
    if (!buffer.empty())
    {
        result.push_back(buffer);
    }
    return result;
}

static string format(const char* format, va_list ap)
{
    char* msg = new char[MAX_FORMAT_LENGTH];
    vsprintf(msg, format, ap);
    string result = msg;
    delete[] msg;

    return result;
}

static void fail(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    string result = format(fmt, ap);
    va_end(ap);
    fprintf(stderr, "FAIL: %s\n", result.c_str());
    exit(EXIT_FAILURE);
}

void redirectFd(int fd, const char* const fileName, const char* const mode)
{
    FILE* fp = fopen(fileName != NULL && strlen(fileName) ? fileName : "/dev/null", mode);
    
    if (!fp) 
        fail("Can't setup redirection from/to \"%s\"", fileName);
    
    if (dup2(fileno(fp), fd) == -1) 
        fail("Can't setup descriptor for redirection from/to \"%s\"", fileName);
}

struct Subprocess {
    pid_t pid;
    
    string commandLine;
    string currentDirectory;
    
    bool checkIdleness;
    bool restrictUi;
    
    uint64_t timeLimit;
    uint64_t memoryLimit;
    
    unsigned int processLimit;
    
    struct SubprocessResult subprocessResult;
    string redirectFiles[REDIRECT_LAST];
};

struct Subprocess * USED Subprocess_CreateEx(
    void* (*mallocfunc)(size_t),
    void* (*reallocfunc)(void*, size_t),
    void (*freefunc)(void*)) {
        return NULL;
}

struct Subprocess * USED Subprocess_Create() {
    return new Subprocess();
}

void USED Subprocess_Destroy(struct Subprocess * self) {
    delete self;
}

struct SubprocessErrorEntry USED Subprocess_PopError(struct Subprocess * const self) {
  struct SubprocessErrorEntry result;
  memset(&result, 0, sizeof(result));
  return result;
}

int USED Subprocess_HasError(const struct Subprocess * const self) {
  return 0;
}

int USED Subprocess_SetStringA(
    struct Subprocess * const self,
    const enum SUBPROCESS_PARAM param,
    const char * const cValue) {
        switch (param)
        {
            case RUNLIB_CURRENT_DIRECTORY:
                self->currentDirectory = cValue;
                return 1;
            case RUNLIB_COMMAND_LINE:
                self->commandLine = cValue;
                return 1;
            default:
                return 0;
        }
}

int USED Subprocess_SetStringW(
    struct Subprocess * const self,
    const enum SUBPROCESS_PARAM param,
    const wchar_t * const wValue) {
    
	return 0;
}

int USED Subprocess_SetStringWB(
    struct Subprocess * const self,
    const enum SUBPROCESS_PARAM param,
    const wchar_t * const wValue) {

	return 0;
}

int USED Subprocess_SetInt(
    struct Subprocess * const self, const enum SUBPROCESS_PARAM param, const uint64_t iValue) {
    switch (param)
    {
        case RUNLIB_TIME_LIMIT:
            self->timeLimit = iValue;
            return 1;
        case RUNLIB_MEMORY_LIMIT:
            self->memoryLimit = iValue;
            return 1;
        case RUNLIB_PROCESS_LIMIT:
            self->processLimit = iValue;
            return 1;
        default:
            return 0;
    }
}

int USED Subprocess_SetBool(
    struct Subprocess * const self, const enum SUBPROCESS_PARAM param, const int bValue) {
    switch (param)
    {
        case RUNLIB_CHECK_IDLENESS:
            self->checkIdleness = bValue;
            return 1;
        case RUNLIB_RESTRICT_UI:
            self->restrictUi = bValue;
            return 1;
        default:
            return 0;
    }
}

int USED Subprocess_SetCallback(
    struct Subprocess * const self, const SubprocessCbFunc cb, void * const cbarg) {
  return 0;
}

int USED Subprocess_SetFileRedirectW(struct Subprocess * const self, const enum REDIRECTION_KEY key, const wchar_t * const wFileName) {
  return 0;
}

int USED Subprocess_SetFileRedirectA(struct Subprocess * const self, const enum REDIRECTION_KEY key, const char * const cFileName) {
    self->redirectFiles[key] = cFileName;
    return 1;
}


int USED Subprocess_SetBufferOutputRedirect(struct Subprocess * const self, const enum REDIRECTION_KEY key) {
  return 0;
}


int USED Subprocess_Launch(struct Subprocess * const self) {
    fail("Subprocess_Launch currently doesn't supported, use Subprocess_Start");
    return 0;
}

int USED Subprocess_StartEx(struct Subprocess * const self) {
    fail("Subprocess_StartEx currently doesn't supported, use Subprocess_Start");
    return 0;
}

static void Subprocess_LaunchChild(struct Subprocess * const self)
{
    // Sets current directory
    if (!self->currentDirectory.empty()) 
    {
        if (chdir(self->currentDirectory.c_str()))
            fail("Unable to change current directory to \"%s\"", self->currentDirectory.c_str());
    }
    
    // Redirects standard file descriptors
    redirectFd(0, self->redirectFiles[0].c_str(), "rt");
    redirectFd(1, self->redirectFiles[1].c_str(), "wt");
    redirectFd(2, self->redirectFiles[2].c_str(), "wt");
    
    vector<string> tokens = tokenizeCommandLine(self->commandLine);
    if (tokens.size() == 0)
        fail("Command line is empty");
    
    char* program = strdup(tokens[0].c_str());
    int argc = tokens.size();
    char** argv = new char*[argc + 1];
    for (int i = 0; i < argc; i++)
        argv[i] = strdup(tokens[i].c_str());
    argv[argc] = NULL;
    
    char** envp = new char*[2];
    envp[0] = strdup((string("PATH=") + getenv("PATH")).c_str());
    envp[1] = NULL;
    
    environ = envp; 
    execvp(program, argv);
    
    free(envp[0]);
    for (int i = 0; i < argc; i++)
        free(argv[i]);
    free(program);
    delete[] envp;
    delete[] argv;
    
    exit(EXIT_FAILURE);
}

static void Subprocess_WaitForChild(struct Subprocess * const self)
{
    while (self->subprocessResult.SuccessCode == 0) {
            int status;
            pid_t wres;
            struct rusage usage;

            wres = wait4(self->pid, &status, 0, &usage);
            if (wres == -1 && errno == EINTR) {
                self->subprocessResult.SuccessCode = EF_KILLED;
                self->subprocessResult.ExitCode = WEXITSTATUS(status);
                return;
            } else if (wres == self->pid && WIFSTOPPED(status)) {
                self->subprocessResult.SuccessCode = EF_KILLED;
                self->subprocessResult.ExitCode = WEXITSTATUS(status);
                return;
            } else if (wres == self->pid && WIFEXITED(status)) {
                self->subprocessResult.SuccessCode = 0;
                self->subprocessResult.ExitCode = WEXITSTATUS(status);
                return;
            } else if (wres == self->pid && WIFSIGNALED(status)) {
                self->subprocessResult.SuccessCode = EF_KILLED;
                self->subprocessResult.ExitCode = WEXITSTATUS(status);
                return;
            } else {
                fail("Child: unexpected wres");
            }
        }
}

int USED Subprocess_Start(struct Subprocess * const self) {
    if (!(self->pid = fork())) {
        Subprocess_LaunchChild(self);
    }
    return 1;
}

int USED Subprocess_Wait(struct Subprocess * const self) {
    Subprocess_WaitForChild(self);
    return 1;
}


const struct SubprocessResult * const USED Subprocess_GetResult(const struct Subprocess * const self) {
    if (self == NULL)
        return NULL;
    return &self->subprocessResult;
}


const struct SubprocessPipeBuffer * const USED Subprocess_GetRedirectBuffer(const struct Subprocess * const self, enum REDIRECTION_KEY key) {
  return NULL;
}
