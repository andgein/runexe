#include "InvocationResult.h"
#include "InvocationVerdict.h"
#include "Configuration.h"
#include "Run.h"

#include "w32invoke.h"

#include <string>

using namespace runexe;
using namespace std;

InvocationResult::InvocationResult(const SubprocessResult* const invocationResult)
{
    setDefaults();

    int flags = invocationResult->SuccessCode;

    if (0 == flags)
    {
        verdict = SUCCESS;
        comment = "success";
    }
    else if (0 != (flags & EF_PROCESS_LIMIT_HIT) || 0 != (flags & EF_PROCESS_LIMIT_HIT_POST))
    {
        verdict = SECURITY_VIOLATION;
        comment = "process limit violated";
    }
    else if (0 != (flags & EF_INACTIVE) || 0 != (flags & EF_TIME_LIMIT_HARD))
    {
        verdict = IDLENESS_LIMIT_EXCEEDED;
        comment = "idleness limit exceeded";
    }
    else if (0 != (flags & EF_TIME_LIMIT_HIT) || 0 != (flags & EF_TIME_LIMIT_HIT_POST))
    {
        verdict = TIME_LIMIT_EXCEEDED;
        comment = "time limit exceeded";
    }
    else if (0 != (flags & EF_MEMORY_LIMIT_HIT) || 0 != (flags & EF_MEMORY_LIMIT_HIT_POST))
    {
        verdict = MEMORY_LIMIT_EXCEEDED;
        comment = "memory limit exceeded";
    }
    else
    {
        verdict = CRASH;
        comment = "invocation crashed";
    }

    if (verdict == CRASH)
        return;

    exitCode = invocationResult->ExitCode;
    time = (int)((invocationResult->ttKernel + invocationResult->ttUser) / 1000LL);
    passedTime = (int)(invocationResult->ttWall / 1000LL);
    memory = invocationResult->PeakMemory;    
}

InvocationResult::InvocationResult(const InvocationVerdict& invocationVerdict,
                                   const string& comment)
{
    if (invocationVerdict != CRASH && invocationVerdict != FAIL)
        fail("Crash/Fail InvocationResult constructor invoked for verdict " +
                invocationVerdictToString(invocationVerdict));

    setDefaults();

    verdict = invocationVerdict;
    this->comment = comment;
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

void InvocationResult::setDefaults()
{
    verdict = FAIL;
    exitCode = RUN_EXIT_FAILURE;
    time = 0;
    memory = 0;
    passedTime = 0;
    comment = "";
}
