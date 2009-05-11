#include "InvocationParams.h"
#include "Configuration.h"
#include "Run.h"
#include "Strings.h"

using namespace runexe;
using namespace std;

void InvocationParams::setDefaults()
{
    timeLimit = INFINITE_LIMIT_INT64;
    memoryLimit = INFINITE_LIMIT_INT64;
    redirectInput = "";
    redirectOutput = "";
    redirectError = "";
    homeDirectory = "";
    commandLine = "";
    userName = "";
    domain = "";
    password = "";
    trustedProcess = false;
    idlenessChecking = true;
}

InvocationParams::InvocationParams(const vector<string>& cmdLineParams)
{
    setDefaults();

    size_t tokensCount = cmdLineParams.size();

    Configuration& configuration = Configuration::getConfiguration();

    for (size_t currentTokenNumber = 1; currentTokenNumber < tokensCount; ++currentTokenNumber)
    {
        string currentToken = cmdLineParams[currentTokenNumber];        

        if (currentToken == "-t")
        {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected time limit value after \"-t\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            timeLimit = parseTimeLimit(currentToken);

            continue;
        }

        if (currentToken == "-m")
        {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected memory limit value after \"-m\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            memoryLimit = parseMemoryLimit(currentToken);            

            continue;
        }

        if (currentToken == "-d")
        {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected directory after \"-d\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            homeDirectory = currentToken;

            continue;
        }

        if (currentToken == "-l")
        {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected login name after \"-l\"");

            currentToken = cmdLineParams[++currentTokenNumber];

            size_t slashIdx = currentToken.find("/");
            if (slashIdx != string::npos)
            {
                userName = currentToken.substr(0, slashIdx);
                domain = currentToken.substr(slashIdx + 1);
            }
            else
            {
                slashIdx = currentToken.find("\\");
                if (slashIdx != string::npos)
                {
                    userName = currentToken.substr(0, slashIdx);
                    domain = currentToken.substr(slashIdx + 1);
                }
                else
                    userName = currentToken;
            }
            
            continue;
        }

        if (currentToken == "-p")
        {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected password after \"-p\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            password = currentToken;

            continue;
        }

        if (currentToken == "-i")
        {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected file name after \"-i\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            redirectInput = currentToken;

            continue;
        }

        if (currentToken == "-o")
        {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected file name after \"-o\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            redirectOutput = currentToken;

            continue;
        }

        if (currentToken == "-e")
        {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected file name after \"-e\"");

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
            fail("not implemented option \"-s\"");
            /*
            if (currentTokenNumber == tokensCount - 1)
            crash("expected file name after \"-s\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            configuration.setStatisticsFileName(currentToken);

            continue;
            */
        }

        if (currentToken == "-D")
        {
            fail("not implemented option \"-D\"");
            /*
            if (currentTokenNumber == tokensCount - 1)
            crash("expected var=value after \"-s\"");

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
                crash("expected file name after \"--xml-to-file\"");

            currentToken = cmdLineParams[++currentTokenNumber];
            configuration.setXmlOutput(true);
            configuration.setXmlFileName(currentToken);

            continue;
        }

        if (currentToken == "--no-idleness-check")
        {
            idlenessChecking = false;

            continue;
        }

        commandLine = buildCommandLine(cmdLineParams, currentTokenNumber);

        return;
    }

    crash("unexpected end of parameters");
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

    // In milliseconds (suffix "ms").
    if (s.length() > 2 && s.find("ms") == s.length() - 2)
    {
        timeLimit = Strings::parseInt64(s.substr(0, s.length() - 2));

        if (timeLimit < 0 || timeLimit >= INFINITE_LIMIT / 1000)
            crash("invalid time limit");
    }
    else    
    {
        // In seconds (suffix "s").
        if (s.length() > 1 && s[s.length() - 1] == 's')
            s = s.substr(0, s.length() - 1);

        // In seconds (no suffix).
        timeLimit = Strings::parseInt64(s);

        if (timeLimit < 0 || timeLimit >= INFINITE_LIMIT)
            crash("invalid time limit");

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
            crash("invalid memory limit");

        memoryLimit *= 1024;
    }
    // In mebibytes (suffix "M").
    else if (s.find("M") == s.length() - 1 || s.find("m") == s.length() - 1)
    {
        memoryLimit = Strings::parseInt64(s.substr(0, s.length() - 1));

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64 / 1024 / 1024)
            crash("invalid memory limit");

        memoryLimit *= 1024 * 1024;
    }
    // In bytes (no suffix).
    else
    {
        memoryLimit = Strings::parseInt64(s);

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64)
            crash("invalid memory limit");
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

string InvocationParams::getUserName() const
{
    return userName;
}

string InvocationParams::getDomain() const
{
    return domain;
}

string InvocationParams::getPassword() const
{
    return password;
}

bool InvocationParams::isIdlenessChecking() const
{
    return idlenessChecking;
}
