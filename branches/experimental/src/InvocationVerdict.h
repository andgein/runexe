#ifndef _INVOCATION_VERDICT_H_
#define _INVOCATION_VERDICT_H_

#include <string>

namespace runexe
{
    enum InvocationVerdict
    {
        SUCCESS,
        FAIL,
        CRASH,
        TIME_LIMIT_EXCEEDED,
        MEMORY_LIMIT_EXCEEDED,
        IDLENESS_LIMIT_EXCEEDED,
        SECURITY_VIOLATION
    };

    std::string invocationVerdictToString(InvocationVerdict verdict);
}

#endif
