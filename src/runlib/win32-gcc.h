#ifndef __WIN32_GCC__
#define __WIN32_GCC__

#if defined(__GNUC__)
#define __const__ __attribute__((const))
#define __pure__ __attribute__((pure))
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#define USED __attribute__((used))
#else
#define __const__
#define __pure__
#define likely(x) (x)
#define unlikely(c) (x)
#define USED
#endif

#endif