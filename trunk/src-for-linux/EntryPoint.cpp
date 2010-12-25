#include "../src/Run.h"
#include "../src/Configuration.h"
#include "../src/InvocationResult.h"
#include "../src/Strings.h"

#include "process.h"

#include <string>
#include <cstdlib>
#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <cstdio>

using namespace runexe;
using namespace std;

InvocationVerdict verdictByState(const process_state& state)
{
    if (state == BEFORE || state == RUNNING || state == FAILED)
        return FAIL;
        
    if (state == EXITED)
        return SUCCESS;
    
    if (state == TIME_EXCEEDED)
        return TIME_LIMIT_EXCEEDED;
    
    if (state == MEMORY_EXCEEDED)
        return MEMORY_LIMIT_EXCEEDED;
        
    if (state == IDLENESS_EXCEEDED)
        return IDLENESS_LIMIT_EXCEEDED;
        
    fail("Unexpected state");
    return FAIL;
}

int main(int argc, char* argv[])
{
    InvocationParams invocationParams = processCommandLine(argc, argv);
    
    process_params params;
    string commandLine = invocationParams.getCommandLine();
    
    if (invocationParams.isIdlenessChecking())
	params.check_idleness = true;

    long long timeLimit = invocationParams.getTimeLimit();
    if (timeLimit != InvocationParams::INFINITE_LIMIT_INT64)
        params.time_limit = timeLimit;
    
    long long memoryLimit = invocationParams.getMemoryLimit();
    if (memoryLimit != InvocationParams::INFINITE_LIMIT_INT64)
        params.memory_limit = memoryLimit;

    string redirectInput = invocationParams.getRedirectInput();
    if (!redirectInput.empty())
        params.input_file = redirectInput;

    string redirectOutput = invocationParams.getRedirectOutput();
    if (!redirectOutput.empty())
        params.output_file = redirectOutput;

    string redirectError = invocationParams.getRedirectError();
    if (!redirectError.empty())
        params.error_file = redirectError;

    string homeDirectory = invocationParams.getHomeDirectory();
    if (!homeDirectory.empty())
        params.directory = homeDirectory;

    string userName = invocationParams.getUserName();
    if (!userName.empty())
        fail("can't set user name to '" + userName + "' [unsupported]");

    string domain = invocationParams.getDomain();
    if (!domain.empty())
        fail("can't set domain to '" + domain + "' [unsupported]");

    string password = invocationParams.getPassword();
    if (!password.empty())
        fail("can't set password to '" + password + "' [unsupported]");

    string injectDll = invocationParams.getInjectDll();
    if (!injectDll.empty())
        fail("can't set injectDll to '" + injectDll + "' [unsupported]");

    bool isTrusted = invocationParams.isTrustedProcess();
    if (isTrusted)
        fail("can't set trusted mode [unsupported]");

    process_outcome outcome = run(commandLine, params);

    InvocationResult invocationResult;
    
    invocationResult.setExitCode(outcome.exit_code);
    invocationResult.setComment(outcome.comment);
    invocationResult.setInvocationVerdict(verdictByState(outcome.state));
    invocationResult.setUserTime(outcome.time);
    invocationResult.setMemory(outcome.memory);
    invocationResult.setPassedTime(outcome.passed_time);
    
    Configuration& configuration = Configuration::getConfiguration();

    if (configuration.isScreenOutput())
        printInvocationResult(invocationParams, invocationResult);

    if (configuration.isXmlOutput())
        printXmlInvocationResult(invocationResult, configuration.getXmlFileName());
        
    if (redirectOutput.empty())
	remove(outcome.output_file.c_str());
	
    if (redirectError.empty())
	remove(outcome.error_file.c_str());

    if (configuration.isReturnExitCode())
        return invocationResult.getExitCode();

    return EXIT_SUCCESS;
}

