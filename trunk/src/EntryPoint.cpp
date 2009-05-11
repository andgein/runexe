extern "C"
{
#include "runlib/w32invoke.h"
}

#include "Run.h"
#include "Configuration.h"
#include "InvocationResult.h"
#include "Strings.h"

#include <string>
#include <cstdlib>

using namespace runexe;
using namespace std;

int main(int argc, char* argv[])
{
    InvocationParams invocationParams = processCommandLine(argc, argv);

    Subprocess* process = Subprocess_Create();

    if (invocationParams.isIdlenessChecking())
        if (0 == Subprocess_SetBool(process, RUNLIB_CHECK_IDLENESS, 1))
            error("can't set idleness checking");

    string commandLine = invocationParams.getCommandLine();
    if (0 == Subprocess_SetStringA(process, RUNLIB_COMMAND_LINE, commandLine.c_str()))
        error("can't set process command line to '" + commandLine + "'");

    long long timeLimit = invocationParams.getTimeLimit();
    if (timeLimit != InvocationParams::INFINITE_LIMIT_INT64)
        if (0 == Subprocess_SetInt(process, RUNLIB_TIME_LIMIT, timeLimit * 10000LL))
            error("can't set time limit to " + Strings::integerToString(timeLimit));

    long long memoryLimit = invocationParams.getMemoryLimit();
    if (memoryLimit != InvocationParams::INFINITE_LIMIT_INT64)
        if (0 == Subprocess_SetInt(process, RUNLIB_MEMORY_LIMIT, memoryLimit))
            error("can't set memory limit to " + Strings::integerToString(memoryLimit));

    string redirectInput = invocationParams.getRedirectInput();
    if (!redirectInput.empty())
        if (0 == Subprocess_SetFileRedirectA(process, Input, redirectInput.c_str()))
            error("can't set input redirection to '" + redirectInput + "'");

    string redirectOutput = invocationParams.getRedirectOutput();
    if (!redirectOutput.empty())
        if (0 == Subprocess_SetFileRedirectA(process, Output, redirectOutput.c_str()))
            error("can't set output redirection to '" + redirectOutput + "'");

    string redirectError = invocationParams.getRedirectError();
    if (!redirectError.empty())
        if (0 == Subprocess_SetFileRedirectA(process, Error, redirectError.c_str()))
            error("can't set error redirection to '" + redirectError + "'");

    string homeDirectory = invocationParams.getHomeDirectory();
    if (!homeDirectory.empty())
        if (0 == Subprocess_SetStringA(process, RUNLIB_CURRENT_DIRECTORY, homeDirectory.c_str()))
            error("can't set home directory to '" + homeDirectory + "'");

    string userName = invocationParams.getUserName();
    if (!userName.empty())
        if (0 == Subprocess_SetStringA(process, RUNLIB_USERNAME, userName.c_str()))
            error("can't set user name to '" + userName + "'");

    string domain = invocationParams.getDomain();
    if (!domain.empty())
        if (0 == Subprocess_SetStringA(process, RUNLIB_DOMAIN, domain.c_str()))
            error("can't set domain to '" + domain + "'");

    string password = invocationParams.getPassword();
    if (!password.empty())
        if (0 == Subprocess_SetStringA(process, RUNLIB_PASSWORD, password.c_str()))
            error("can't set password to '" + password + "'");

    if (0 == Subprocess_Start(process))
        error("can't execute '" + invocationParams.getCommandLine() + "'");

    if (0 == Subprocess_Wait(process))
        error("can't wait for process");

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
