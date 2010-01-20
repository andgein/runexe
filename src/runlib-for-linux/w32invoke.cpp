#define _UNICODE

#include <wchar.h>

#include <cstdio>
#include <stdint.h>
#include <cstring>
#include <stdbool.h>
#include <memory>

#include "w32invoke.h"

#define USED

using namespace std;

struct Subprocess {
  void* (*mallocfunc)(size_t);
  void* (*reallocfunc)(void*, size_t);
  void (*freefunc)(void*);
};

static void SubprocessWipe(struct Subprocess * const self) {
  memset(self, 0, sizeof(*self));
}

struct Subprocess * USED Subprocess_CreateEx(
	void* (*mallocfunc)(size_t),
	void* (*reallocfunc)(void*, size_t),
	void (*freefunc)(void*)) {
  struct Subprocess * const result = (Subprocess *)(mallocfunc ? mallocfunc(sizeof(struct Subprocess)) : malloc(sizeof(struct Subprocess)));

  if (result) {
    SubprocessWipe(result);
    result->mallocfunc = mallocfunc;
    result->reallocfunc = reallocfunc;
    result->freefunc = freefunc;
  }

  return result;
}

struct Subprocess * USED Subprocess_Create() {
  return Subprocess_CreateEx(NULL, NULL, NULL);
}

void USED Subprocess_Destroy(struct Subprocess * self) {
  if (self->freefunc)
    self->freefunc(self);
  else
    free(self);
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

	return 0;
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
  return 0;
}

int USED Subprocess_SetBool(
    struct Subprocess * const self, const enum SUBPROCESS_PARAM param, const int bValue) {
  return 0;
}

int USED Subprocess_SetCallback(
    struct Subprocess * const self, const SubprocessCbFunc cb, void * const cbarg) {
  return 0;
}

int USED Subprocess_SetFileRedirectW(struct Subprocess * const self, const enum REDIRECTION_KEY key, const wchar_t * const wFileName) {
  return 0;
}

int USED Subprocess_SetFileRedirectA(struct Subprocess * const self, const enum REDIRECTION_KEY key, const char * const cFileName) {
  return 0;
}


int USED Subprocess_SetBufferOutputRedirect(struct Subprocess * const self, const enum REDIRECTION_KEY key) {
  return 0;
}


int USED Subprocess_Launch(struct Subprocess * const self) {
  return 0;
}

int USED Subprocess_StartEx(struct Subprocess * const self) {
  return 0;
}

int USED Subprocess_Start(struct Subprocess * const self) {
  return 0;
}

int USED Subprocess_Wait(struct Subprocess * const self) {
  return 0;
}


const struct SubprocessResult * const USED Subprocess_GetResult(const struct Subprocess * const self) {
  return NULL;
}


const struct SubprocessPipeBuffer * const USED Subprocess_GetRedirectBuffer(const struct Subprocess * const self, enum REDIRECTION_KEY key) {
  return NULL;
}
