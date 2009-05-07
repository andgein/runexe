#include "InvocationParams.h"
#include "Configuration.h"
#include "Run.h"
#include "Strings.h"

using namespace runexe;
using namespace std;

InvocationParams::InvocationParams(const vector<string>& cmdLineParams)
{
    trustedProcess = false;

    size_t tokensCount = cmdLineParams.size();

    Configuration& configuration = Configuration::getConfiguration();

    for (size_t currentTokenNumber = 1; currentTokenNumber < tokensCount; ++currentTokenNumber)
    {
        string currentToken = cmdLineParams[currentTokenNumber];        

        if (currentToken == "-t")
        {
            if (currentTokenNumber == tokensCount - 1)
                error("expected time limit value after \"-t\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            timeLimit = parseTimeLimit(currentToken) * 10000LL;

            continue;
        }

        if (currentToken == "-m")
        {
            if (currentTokenNumber == tokensCount - 1)
                error("expected memory limit value after \"-m\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            memoryLimit = parseMemoryLimit(currentToken);            

            continue;
        }

        if (currentToken == "-d")
        {
            if (currentTokenNumber == tokensCount - 1)
                error("expected directory after \"-d\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            homeDirectory = currentToken;

            continue;
        }

        if (currentToken == "-l")
        {
            error("not implemented option \"-l\"");
            /*
            if (currentTokenNumber == tokensCount - 1)
            error("expected login name after \"-l\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            invocationParams.setUserLogin(currentToken);

            continue;
            */
        }

        if (currentToken == "-p")
        {
            error("not implemented option \"-p\"");
            /*
            if (currentTokenNumber == tokensCount - 1)
            error("expected password after \"-p\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            invocationParams.setUserPassword(currentToken);

            continue;
            */
        }

        if (currentToken == "-i")
        {
            if (currentTokenNumber == tokensCount - 1)
                error("expected file name after \"-i\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            redirectInput = currentToken;

            continue;
        }

        if (currentToken == "-o")
        {
            if (currentTokenNumber == tokensCount - 1)
                error("expected file name after \"-o\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            redirectOutput = currentToken;

            continue;
        }

        if (currentToken == "-e")
        {
            if (currentTokenNumber == tokensCount - 1)
                error("expected file name after \"-e\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            redirectError = currentToken;

            continue;
        }

        if (currentToken == "-x")
        {
            configuration.setReturnExitCode(true);

            continue;
        }

        if (currentToken == "-q")
        {
            configuration.setScreenOutput(false);

            continue;
        }


        if (currentToken == "-z")
        {
            trustedProcess = true;

            continue;
        }

        if (currentToken == "-s")
        {
            error("not implemented option \"-s\"");
            /*
            if (currentTokenNumber == tokensCount - 1)
            error("expected file name after \"-s\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            configuration.setStatisticsFileName(currentToken);

            continue;
            */
        }

        if (currentToken == "-D")
        {
            error("not implemented option \"-D\"");
            /*
            if (currentTokenNumber == tokensCount - 1)
            error("expected var=value after \"-s\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            ////////////////////////////////////////////

            continue;
            */
        }

        if (currentToken == "-xml" || currentToken == "--xml")
        {
            configuration.setScreenOutput(false);
            configuration.setXmlOutput(true);

            continue;
        }

        if (currentToken == "-xmltof" || currentToken == "--xml-to-file")
        {
            if (currentTokenNumber == tokensCount - 1)
                error("expected file name after \"--xml-to-file\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            configuration.setXmlOutput(true);
            configuration.setXmlFileName(currentToken);

            continue;
        }

        commandLine = buildCommandLine(cmdLineParams, currentTokenNumber);

        return;
    }

    error("unexpected end of parameters");
}

string InvocationParams::buildCommandLine(const vector<string>& params, size_t fromIndex)
{
    size_t paramsSize = params.size();
    string result = "";

    for (size_t currentTokenNumber = fromIndex; currentTokenNumber < paramsSize; ++currentTokenNumber)
    {
        string currentToken = params[currentTokenNumber];

        if (currentTokenNumber > fromIndex)
            result += " ";

        result += currentToken;
    }

    return result;
}

long long InvocationParams::parseTimeLimit(const string& _s)
{
    string s(_s);

    long long timeLimit;

    // In seconds (suffix "s").
    if (s.length() > 1 && s[s.length() - 1] == 's')
        s = s.substr(0, s.length() - 1);

    // In milliseconds (suffix "ms").
    if (s.length() > 2 && s.find("ms") == s.length() - 2)
    {
        timeLimit = Strings::parseInt64(s.substr(0, s.length() - 2));

        if (timeLimit < 0 || timeLimit >= INFINITE_LIMIT / 1000)
            error("invalid time limit");
    }
    else
    // In seconds (no suffix).
    {
        timeLimit = Strings::parseInt64(s);

        if (timeLimit < 0 || timeLimit >= INFINITE_LIMIT)
            error("invalid time limit");

        timeLimit *= 1000;
    }

    return timeLimit;
}

long long InvocationParams::parseMemoryLimit(const string& s)
{
    long long memoryLimit;

    // In kibibytes (suffix "K").
    if (s.find("K") == s.length() - 1 || s.find("k") == s.length() - 1)
    {
        memoryLimit = Strings::parseInt64(s.substr(0, s.length() - 1));

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64 / 1024)
            error("invalid memory limit");

        memoryLimit *= 1024;
    }
    // In mebibytes (suffix "M").
    else if (s.find("M") == s.length() - 1 || s.find("m") == s.length() - 1)
    {
        memoryLimit = Strings::parseInt64(s.substr(0, s.length() - 1));

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64 / 1024 / 1024)
            error("invalid memory limit");

        memoryLimit *= 1024 * 1024;
    }
    // In bytes (no suffix).
    else
    {
        memoryLimit = Strings::parseInt64(s);

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64)
            error("invalid memory limit");
    }

    return memoryLimit;
}

long long InvocationParams::getTimeLimit() const
{
    return timeLimit;
}

long long InvocationParams::getMemoryLimit() const
{
    return memoryLimit;
}

string InvocationParams::getRedirectInput() const
{
    return redirectInput;
}

string InvocationParams::getRedirectOutput() const
{
    return redirectOutput;
}

string InvocationParams::getRedirectError() const
{
    return redirectError;
}

string InvocationParams::getHomeDirectory() const
{
    return homeDirectory;
}

string InvocationParams::getCommandLine() const
{
    return commandLine;
}

bool InvocationParams::isTrustedProcess() const
{
    return trustedProcess;
}
