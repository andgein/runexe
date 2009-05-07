#include "InvocationResult.h"
#include "InvocationVerdict.h"

extern "C"
{
#include "runlib/w32invoke.h"
}

#include <string>

using namespace runexe;
using namespace std;

InvocationResult::InvocationResult(const SubprocessResult* const invocationResult)
{
    int flags = invocationResult->SuccessCode;

    if (0 == flags)
        verdict = SUCCESS;
    else if (0 != (flags & EF_INACTIVE) || 0 != (flags & EF_TIME_LIMIT_10X))
        verdict = IDLENESS_LIMIT_EXCEEDED;
    else if (0 != (flags & EF_TIME_LIMIT_HIT) || 0 != (flags & EF_TIME_LIMIT_HIT_POST))
        verdict = TIME_LIMIT_EXCEEDED;
    else if (0 != (flags & EF_MEMORY_LIMIT_HIT) || 0 != (flags & EF_MEMORY_LIMIT_HIT_POST))
        verdict = MEMORY_LIMIT_EXCEEDED;
    else
        verdict = CRASH;

    exitCode = invocationResult->ExitCode;
    time = (int)((invocationResult->ttKernel + invocationResult->ttUser) / 10000LL);
    passedTime = (int)(invocationResult->ttWall / 10000LL);
    memory = invocationResult->PeakMemory;
    // TODO: comment
    comment = "";
}

InvocationVerdict InvocationResult::getInvocationVerdict() const
{
    return verdict;
}

int InvocationResult::getExitCode() const
{
    return exitCode;
}

int InvocationResult::getTime() const
{
    return time;
}

long long InvocationResult::getMemory() const
{
    return memory;
}

string InvocationResult::getComment() const
{
    return comment;
}

int InvocationResult::getPassedTime() const
{
    return passedTime;
}
