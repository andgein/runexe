#include "Run.h"
#include "Strings.h"

#include <string>
#include <vector>
#include <iostream>

using namespace runexe;
using namespace std;

void runexe::quit(unsigned int exitCode)
{
    exit(exitCode);
}

void runexe::showInfo()
{
    cout << endl
        << "RunExe for Windows NT, Version " << version << endl
        << "Copyright(c) Saratov SU Codecenter Development Team, "
        << copyrightYears << endl
        << "Based on runlib by Paul P. Komkoff Jr." << endl
        << "Written by Dmitry Levshunov" << endl << endl
        << "Use \"runexe -h\" to get help information" << endl;    
}

void runexe::showHelp()
{
    showInfo();

    cout << endl
        << "This program runs other program for given period of time with specified" << endl
        << "  memory restrictions" << endl << endl
        << "Command line format:" << endl
        << "  runexe [<options>] <program> [<parameters>]" << endl
        << "Where options are:" << endl
        << "  -h               - show this help" << endl
        << "  -t <time-limit>  - time limit, terminate after <time-limit> seconds, you can" << endl
        << "                     add \"ms\" (without quotes) after the number to specify" << endl
        << "                     time limit in milliseconds" << endl
        << "  -m <mem-limit>   - memory limit, terminate if working set of the process" << endl
        << "                     exceeds <mem-limit> bytes, you can add K or M to specify" << endl
        << "                     memory limit in kilo- or megabytes" << endl
        << "  -d <directory>   - make <directory> home directory for process" << endl
        << "  -l <login-name>  - create process under <login-name>" << endl
        << "  -p <password>    - logins user using <password>" << endl
        << "  -j <file>        - injects specified dll file into the process" << endl
        << "  -i <file>        - redirects standart input stream to the <file>" << endl
        << "  -o <file>        - redirects standart output stream to the <file>" << endl
        << "  -e <file>        - redirects standart error stream to the <file>" << endl
        << "  -x               - return exit code of the application" << endl
        << "  -q               - do not display any information on the screen" << endl
        << "  -s <file>        - store statistics in then <file>" << endl
        << "  -D var=value     - sets value of the environment variable, current environment" << endl
        << "                     is completely ignored in this case" << endl
        << "  -z               - run process in trusted mode" << endl
        << "  --show-kernel-mode-time" << endl
        << "                   - show user-mode and kernel-mode time" << endl
        << "                     (shows only user-mode time without this option)" << endl
        << "  --no-idleness-check" << endl
        << "                   - switch off idleness checking" << endl
        << "  --xml, -xml      - form xml document with invocation result information" << endl
        << "                     and display them instead of standard output view" << endl
        << "                     (other output and key \"-q\" ignored)" << endl
        << "  --xml-to-file <file>, -xmltof <file>" << endl
        << "                   - same as \"-xml\" but print xml result into the file <file>" << endl
        << "                     and display usual information form" << endl
        << "Examples:" << endl
        << "  runexe -t 10s -m 32M -i 10s a.exe" << endl
        << "  runexe -d \"C:\\My Directory\" a.exe" << endl
        << "  runexe -l invoker -p password a.exe" << endl
        << "  runexe -i input.txt -o output.txt -e error.txt a.exe" << endl;
}

void runexe::fail(const string& comment)
{
    Configuration& configuration = Configuration::getConfiguration();

    if (configuration.isScreenOutput())
    {
        cout << "Invocation failed" << endl
            << "Comment: " << comment << endl << endl
            << "Use \"runexe -h\" to get help information" << endl;
    }

    InvocationResult invocationResult = InvocationResult(FAIL, comment);

    if (configuration.isXmlOutput())
        printXmlInvocationResult(invocationResult, configuration.getXmlFileName());

    quit();
}

void runexe::crash(const string& comment)
{
    Configuration& configuration = Configuration::getConfiguration();

    if (configuration.isScreenOutput())
    {
        cout << "Invocation crashed" << endl
            << "Comment: " << comment << endl << endl
            << "Use \"runexe -h\" to get help information" << endl;
    }

    InvocationResult invocationResult = InvocationResult(CRASH, comment);

    if (configuration.isXmlOutput())
        printXmlInvocationResult(invocationResult, configuration.getXmlFileName());

    if (configuration.isReturnExitCode())
        quit();
    else
        quit(0);
}

void runexe::crash(const string& comment, Subprocess* const process)
{
    string fullComment = comment;

    bool isFail = false;

    while (0 != Subprocess_HasError(process))
    {
        const SubprocessErrorEntry errorEntry = Subprocess_PopError(process);

        string errorDescription;

        switch (errorEntry.error_id)
        {
        case EID_CREATE_PROCESS_AS_USER:
            errorDescription = "can't create process as user";
            isFail = true;
            break;

        case EID_LOAD_USER_PROFILE:
            errorDescription = "can't load user profile";
            isFail = true;
            break;

        case EID_CREATE_DESKTOP:
            errorDescription = "can't create desktop";
            isFail = true;
            break;

        case EID_CREATE_WINDOW_STATION:
            errorDescription = "can't create window station";
            isFail = true;
            break;

        case EID_LOGON_USER:
            errorDescription = "can't logon user";
            isFail = true;
            break;

        case EID_SET_EXTENDED_LIMIT_INFO:
            errorDescription = "can't set extended limit info";
            isFail = true;
            break;

        case EID_SET_UI_RESTRICTIONS:
            errorDescription = "can't set UI restrictions";
            isFail = true;
            break;

        case EID_GET_PROCESS_TIMES:
            errorDescription = "can't get process times";
            isFail = true;
            break;

        case EID_SYSTEM_TIME_TO_FILE_TIME:
            errorDescription = "can't convert system time to file time";
            isFail = true;
            break;

        case EID_QUERY_JOB_OBJECT:
            errorDescription = "can't query job object";
            isFail = true;
            break;

        case EID_LAUNCH_PROCESS:
            errorDescription = "can't launch process";
            isFail = true;
            break;

        case EID_ASSIGN_TO_JOB:
            errorDescription = "can't assign to job";
            isFail = true;
            break;

        case EID_CREATE_JOB:
            errorDescription = "can't create job";
            isFail = true;
            break;

        default:
            errorDescription = "unknown error";
            isFail = true;
        }

        fullComment += "\n" + Strings::format("Error: '%s', last error code: %d.",
                errorDescription.c_str(), errorEntry.dwLastError);
    }

    if (isFail)
        fail(fullComment);
    else
        crash(fullComment);
}

InvocationParams runexe::processParams(const vector<string>& params)
{
    if (params.size() > 1 && params[1] == "-h")
    {
        showHelp();
        quit();
    }

    return InvocationParams(params);
}

InvocationParams runexe::processCommandLine(int argc, char* argv[])
{
    if (argc <= 1)
    {
        showInfo();
        quit();
    }

    vector<string> params(argc);
    for (int i = 0; i < argc; ++i)
        params[i] = argv[i];

    return processParams(params);
}

void runexe::printTimes(double userTime, double kernelTime,
                        double timeLimit, bool isTimeLimitExceeded)
{
    if (Configuration::getConfiguration().isShowKernelModeTime())
    {
        if (isTimeLimitExceeded)
            cout << "  time consumed:" << endl
                 << "    user mode:   " << userTime << " of " << timeLimit << " sec" << endl
                 << "    kernel mode: " << kernelTime << " sec" << endl;
        else
            cout << "  time consumed:" << endl
                 << "    user mode:   " << userTime << " sec" << endl
                 << "    kernel mode: " << kernelTime << " sec" << endl;
    }
    else
    {
        if (isTimeLimitExceeded)
            cout << "  time consumed: " << userTime << " of " << timeLimit << " sec" << endl;
        else
            cout << "  time consumed: " << userTime << " sec" << endl;
    }
}

void runexe::printInvocationResult(const InvocationParams& invocationParams,
                                   const InvocationResult& invocationResult)
{
    InvocationVerdict invocationVerdict = invocationResult.getInvocationVerdict();

    int exitCode = invocationResult.getExitCode();
    double userTime = (double)invocationResult.getUserTime() / 1000.0;
    double kernelTime = (double)invocationResult.getKernelTime() / 1000.0;
    double passedTime = (double)invocationResult.getPassedTime() / 1000.0;
    long long memory = invocationResult.getMemory();

    double timeLimit = (double)invocationParams.getTimeLimit() / 1000.0;
    long long memoryLimit = invocationParams.getMemoryLimit();

    cout.precision(2);
    cout.setf(ios::fixed);

    switch (invocationVerdict)
    {
    case SUCCESS :
        cout << "Program successfully terminated" << endl
             << "  exit code:     " << exitCode << endl,
        printTimes(userTime, kernelTime),
        cout << "  time passed:   " << passedTime << " sec" << endl
             << "  peak memory:   " << memory << " bytes" << endl;
        break;

    case FAIL :
        fail(invocationResult.getComment());
        break;

    case CRASH :
        crash(invocationResult.getComment());
        break;

    case TIME_LIMIT_EXCEEDED :
        cout << "Time limit exceeded" << endl
             << "Program failed to terminate within " << timeLimit << " sec" << endl,
        printTimes(userTime, kernelTime, timeLimit, true),
        cout << "  time passed:   " << passedTime << " sec" << endl
             << "  peak memory:   " << memory << " bytes" << endl;
        break;

    case MEMORY_LIMIT_EXCEEDED :
        cout << "Memory limit exceeded" << endl
             << "Program tried to allocate more than " << memoryLimit << " bytes" << endl,
        printTimes(userTime, kernelTime),
        cout << "  time passed:   " << passedTime << " sec" << endl
             << "  peak memory:   " << memory << " of " << memoryLimit << " bytes" << endl;
        break;

    case IDLENESS_LIMIT_EXCEEDED :
        cout << "Idleness limit exceeded" << endl
             << "Detected program idle " << endl,
        printTimes(userTime, kernelTime),
        cout << "  time passed:   " << passedTime << " sec" << endl
             << "  peak memory:   " << memory << " bytes" << endl;
        break;

    case SECURITY_VIOLATION :
        cout << "Security violation" << endl
             << "Program tried to do some forbidden action" << endl,
        printTimes(userTime, kernelTime),
        cout << "  time passed:   " << passedTime << " sec" << endl
             << "  peak memory:   " << memory << " bytes" << endl;
        break;

    default :
        fail("unknown invocation verdict");
    }
}

void runexe::printXmlInvocationResult(const InvocationResult& invocationResult,
                                      const string& fileName)
{
    static bool firstCall = true;

    if (!firstCall)
        return;
    else
        firstCall = false;

    vector<string> result;
    result.push_back("<?xml version = \"1.1\" encoding = \"UTF-8\"?>");
    result.push_back("");
    result.push_back("<invocationResult>");
    result.push_back("    <invocationVerdict>" +
            invocationVerdictToString(invocationResult.getInvocationVerdict()) +
            "</invocationVerdict>");
    result.push_back("    <exitCode>" +
            Strings::integerToString(invocationResult.getExitCode()) +
            "</exitCode>");
    result.push_back("    <processorUserModeTime>" +
            Strings::integerToString(invocationResult.getUserTime()) +
            "</processorUserModeTime>");
    result.push_back("    <processorKernelModeTime>" +
        Strings::integerToString(invocationResult.getKernelTime()) +
        "</processorKernelModeTime>");
    result.push_back("    <passedTime>" +
            Strings::integerToString(invocationResult.getPassedTime()) +
            "</passedTime>");
    result.push_back("    <consumedMemory>" +
            Strings::integerToString(invocationResult.getMemory()) +
            "</consumedMemory>");
    result.push_back("    <comment>" + invocationResult.getComment() +
            "</comment>");
    result.push_back("</invocationResult>");

    int resultSize = (int)result.size();

    if (!fileName.empty())
    {
        
        FILE* file;
        fopen_s(&file, fileName.c_str(), "wt");

        if (NULL != file)
        {
            for (int i = 0; i < resultSize; ++i)
                fprintf(file, "%s\n", result[i].c_str());

            fclose(file);
        }
        else
        {
            fail("can't open file '" + fileName + "'");
        }
    }
    else
    {
        for (int i = 0; i < resultSize; ++i)
            printf("%s\n", result[i].c_str());
    }
}
