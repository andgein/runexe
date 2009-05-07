#include "InvocationVerdict.h"

#include <string>

using namespace runexe;
using namespace std;

string runexe::invocationVerdictToString(InvocationVerdict verdict)
{
    switch (verdict)
    {
        case SUCCESS : return "SUCCESS";
        case FAIL : return "FAIL";
        case CRASH : return "CRASH";
        case TIME_LIMIT_EXCEEDED : return "TIME_LIMIT_EXCEEDED";
        case MEMORY_LIMIT_EXCEEDED : return "MEMORY_LIMIT_EXCEEDED";
        case IDLENESS_LIMIT_EXCEEDED : return "IDLENESS_LIMIT_EXCEEDED";
        case SECURITY_VIOLATION : return "SECURITY_VIOLATION";
    }

    return "UNKNOWN";
}
