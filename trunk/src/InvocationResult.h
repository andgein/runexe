#ifndef _INVOCATION_RESULT_H_
#define _INVOCATION_RESULT_H_

#include "InvocationVerdict.h"

#include "w32invoke.h"

#include <string>

namespace runexe
{
    class InvocationResult
    {
    public:
        InvocationResult(const SubprocessResult* const invocationResult);
        InvocationResult(const InvocationVerdict& invocationVerdict,
                const std::string& comment);

        InvocationVerdict getInvocationVerdict() const;
        int getExitCode() const;
        int getUserTime() const;
        int getKernelTime() const;
        long long getMemory() const;
        int getPassedTime() const;
        std::string getComment() const;

        void setInvocationVerdict(const InvocationVerdict& invocationVerdict);
        void setExitCode(int exitCode);
        void setUserTime(int userTime);
        void setKernelTime(int kernelTime);
        void setMemory(long long memory);
        void setPassedTime(int passedTime);
        void setComment(std::string comment);

    private:        
        InvocationVerdict verdict;
        int exitCode;
        int userTime;
        int kernelTime;
        long long memory;
        int passedTime;
        std::string comment;

        void setDefaults();
    };
}

#endif
