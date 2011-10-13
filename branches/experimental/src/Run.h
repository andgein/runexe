#ifndef _RUN_H_
#define _RUN_H_

#include "Configuration.h"
#include "InvocationParams.h"
#include "InvocationResult.h"

#include "w32invoke.h"

#include <string>
#include <vector>

namespace runexe
{
    InvocationParams processCommandLine(int argc, char* argv[]);
    InvocationParams processParams(const std::vector<std::string>& params);    
    void quit(unsigned int exitCode = RUN_EXIT_FAILURE);
    void showInfo();
    void showHelp();
    void crash(const std::string& comment);
    void crash(const std::string& comment, Subprocess* const process);
    void printTimes(double userTime, double kernelTime, double timeLimit = 0.0,
        bool isTimeLimitExceeded = false);
    void fail(const std::string& comment);
    void printInvocationResult(const InvocationParams& invocationParams,
        const InvocationResult& invocationResult);
    void printXmlInvocationResult(const InvocationResult& invocationResult,
        const std::string& fileName);
}

#endif
