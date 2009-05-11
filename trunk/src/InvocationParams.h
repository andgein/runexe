#ifndef _INVOCATION_PARAMS_H_
#define _INVOCATION_PARAMS_H_

#include <string>
#include <vector>

namespace runexe
{
    class InvocationParams
    {
    public:
        static const int INFINITE_LIMIT = 2000000000;
        static const long long INFINITE_LIMIT_INT64 = 1000000000000000000LL;

        InvocationParams(const std::vector<std::string>& cmdLineParams);

        long long getTimeLimit() const;
        long long getMemoryLimit() const;
        std::string getRedirectInput() const;
        std::string getRedirectOutput() const;
        std::string getRedirectError() const;
        std::string getHomeDirectory() const;
        std::string getCommandLine() const;        
        std::string getUserName() const;
        std::string getDomain() const;
        std::string getPassword() const;
        bool isTrustedProcess() const;
        bool isIdlenessChecking() const;

    private:
        long long timeLimit;
        long long memoryLimit;
        std::string redirectInput;
        std::string redirectOutput;
        std::string redirectError;
        std::string homeDirectory;
        std::string commandLine;        
        std::string userName;
        std::string domain;
        std::string password;
        bool trustedProcess;
        bool idlenessChecking;

        std::string buildCommandLine(const std::vector<std::string>& params,
            size_t fromIndex);
        long long parseTimeLimit(const std::string& s);
        long long parseIdlenessLimit(const std::string& s);
        double parseRequiredLoad(const std::string& s);
        long long parseMemoryLimit(const std::string& s);
        void setDefaults();
    };
}

#endif
