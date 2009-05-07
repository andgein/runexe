extern "C"
{
#include "runlib/w32invoke.h"
}

#include "Run.h"
#include "Configuration.h"
#include "InvocationResult.h"

#include <string>
#include <cstdlib>

using namespace runexe;
using namespace std;

int main(int argc, char* argv[])
{  
    InvocationParams invocationParams = processCommandLine(argc, argv);

    Subprocess* process = Subprocess_Create();

    Subprocess_SetBool(process, RUNLIB_CHECK_IDLENESS, 1);

    Subprocess_SetStringA(process, RUNLIB_COMMAND_LINE, invocationParams.getCommandLine().c_str());
    Subprocess_SetInt(process, RUNLIB_TIME_LIMIT, invocationParams.getTimeLimit() * 10000LL);
    Subprocess_SetInt(process, RUNLIB_MEMORY_LIMIT, invocationParams.getMemoryLimit());    

    string redirectInput = invocationParams.getRedirectInput();
    if (!redirectInput.empty())
        Subprocess_SetFileRedirectA(process, Input, redirectInput.c_str());

    string redirectOutput = invocationParams.getRedirectOutput();
    if (!redirectOutput.empty())
        Subprocess_SetFileRedirectA(process, Output, redirectOutput.c_str());

    string redirectError = invocationParams.getRedirectError();
    if (!redirectError.empty())
        Subprocess_SetFileRedirectA(process, Error, redirectError.c_str());

    string homeDirectory = invocationParams.getHomeDirectory();
    if (!homeDirectory.empty())
        Subprocess_SetStringA(process, RUNLIB_CURRENT_DIRECTORY, homeDirectory.c_str());

    Subprocess_Start(process);
    Subprocess_Wait(process);

    InvocationResult invocationResult = InvocationResult(Subprocess_GetResult(process));

    Configuration& configuration = Configuration::getConfiguration();

    if (configuration.isScreenOutput())
        printInvocationResult(invocationParams, invocationResult);

    if (configuration.isXmlOutput())
        printXmlInvocationResult(invocationResult, configuration.getXmlFileName());

    if (configuration.isReturnExitCode())
        return invocationResult.getExitCode();

    return EXIT_SUCCESS;
}
