#ifndef __W32INVOKE__
#define __W32INVOKE__

#include "win32-gcc.h"

struct Subprocess;

enum SUBPROCESS_PARAM {
  RUNLIB_APPLICATION_NAME,
  RUNLIB_COMMAND_LINE,
  RUNLIB_CURRENT_DIRECTORY,
  RUNLIB_ENVIRONMENT,
  RUNLIB_USERNAME,
  RUNLIB_PASSWORD,
  RUNLIB_DOMAIN,
  RUNLIB_TIME_LIMIT,
  RUNLIB_TIME_LIMIT_HARD,
  RUNLIB_MEMORY_LIMIT,
  RUNLIB_PROCESS_LIMIT,
  RUNLIB_CHECK_IDLENESS,
  RUNLIB_RESTRICT_UI,
  RUNLIB_NO_JOB
};

enum SubprocessErrorID {
  EID_CREATE_PROCESS_AS_USER = 1,
  EID_LOAD_USER_PROFILE,
  EID_CREATE_DESKTOP,
  EID_CREATE_WINDOW_STATION,
  EID_LOGON_USER,
  EID_SET_EXTENDED_LIMIT_INFO,
  EID_SET_UI_RESTRICTIONS,
  EID_GET_PROCESS_TIMES,
  EID_SYSTEM_TIME_TO_FILE_TIME,
  EID_QUERY_JOB_OBJECT,
  EID_LAUNCH_PROCESS,
  EID_ASSIGN_TO_JOB,
  EID_CREATE_JOB,
};

enum REDIRECTION_KEY {
  Input = 0,
  Output,
  Error,
  REDIRECT_LAST
};

#define EF_INACTIVE (1 << 0)
#define EF_TIME_LIMIT_HIT (1 << 1)
#define EF_TIME_LIMIT_HARD (1 << 2)
#define EF_MEMORY_LIMIT_HIT (1 << 3)
#define EF_KILLED (1 << 4)
#define EF_STDOUT_OVERFLOW (1 << 5)
#define EF_STDERR_OVERFLOW (1 << 6)
#define EF_STDPIPE_TIMEOUT (1 << 7)
#define EF_TIME_LIMIT_HIT_POST (1 << 8)
#define EF_MEMORY_LIMIT_HIT_POST (1 << 9)
#define EF_PROCESS_LIMIT_HIT (1 << 10)
#define EF_PROCESS_LIMIT_HIT_POST (1 << 11)

struct SubprocessErrorEntry {
  enum SubprocessErrorID error_id;
  uint32_t dwLastError;
};

struct SubprocessResult {
  int SuccessCode;
  int ExitCode;
  uint64_t ttUser, ttKernel, ttWall;
  uint64_t PeakMemory;
  uint64_t TotalProcesses;
};

struct SubprocessPipeBuffer {
  char * cBuffer;
  uint64_t Size;
};

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SubprocessCbFunc)(const struct Subprocess * const, void * const);

#if defined(BUILD_DLL)
#define EXPORTED __declspec(dllexport)
#elif defined(USE_DLL)
#define EXPORTED __declspec(dllimport)
#else
#define EXPORTED
#endif

EXPORTED struct Subprocess * Subprocess_CreateEx(
    void* (*mallocfunc)(size_t),
    void* (*reallocfunc)(void*, size_t),
    void (*freefunc)(void*));
EXPORTED struct Subprocess * Subprocess_Create();
EXPORTED void Subprocess_Destroy(struct Subprocess * self);
EXPORTED int Subprocess_Start(struct Subprocess * const self);

EXPORTED int Subprocess_SetStringW(
    struct Subprocess * const self, const enum SUBPROCESS_PARAM param, const wchar_t * const wValue);
EXPORTED int Subprocess_SetStringWB(
    struct Subprocess * const self, const enum SUBPROCESS_PARAM param, const wchar_t * const wValue);
EXPORTED int Subprocess_SetStringA(
    struct Subprocess * const self, const enum SUBPROCESS_PARAM param, const char * const cValue);
EXPORTED int Subprocess_SetInt(
    struct Subprocess * const self, const enum SUBPROCESS_PARAM param, const uint64_t iValue);
EXPORTED int Subprocess_SetBool(
    struct Subprocess * const self, const enum SUBPROCESS_PARAM param, const int bValue);
EXPORTED int Subprocess_SetCallback(
    struct Subprocess * const self, const SubprocessCbFunc cb, void * const cbarg);
EXPORTED int Subprocess_Wait(struct Subprocess * const self);
EXPORTED const struct SubprocessResult * const Subprocess_GetResult(const struct Subprocess * const self);
EXPORTED int Subprocess_SetFileRedirectW(struct Subprocess * const self, enum REDIRECTION_KEY key, const wchar_t * const wFileName);
EXPORTED int Subprocess_SetFileRedirectA(struct Subprocess * const self, const enum REDIRECTION_KEY key, const char * const cFileName);
EXPORTED int Subprocess_SetBufferOutputRedirect(struct Subprocess * const self, enum REDIRECTION_KEY key);
EXPORTED const struct SubprocessPipeBuffer * const Subprocess_GetRedirectBuffer(const struct Subprocess * const self, enum REDIRECTION_KEY key);

EXPORTED struct SubprocessErrorEntry Subprocess_PopError(struct Subprocess * const self);
EXPORTED int Subprocess_HasError(const struct Subprocess * const self);
#undef EXPORTED

#ifdef __cplusplus
}
#endif


#endif
