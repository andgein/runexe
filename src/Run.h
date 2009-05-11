#ifndef _RUN_H_
#define _RUN_H_

extern "C"
{
#include "runlib/w32invoke.h"
}

#include "Configuration.h"
#include "InvocationParams.h"
#include "InvocationResult.h"

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
    void fail(const std::string& comment);
    void printInvocationResult(const InvocationParams& invocationParams,
        const InvocationResult& invocationResult);
    void printXmlInvocationResult(const InvocationResult& invocationResult,
        const std::string& fileName);
}

#endif
