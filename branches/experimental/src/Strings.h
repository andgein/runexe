#ifndef _RUNEXE_STRINGS_H_
#define _RUNEXE_STRINGS_H_

#include <string>
#include <cstdarg>

namespace runexe
{
    class Strings
    {
    public:
        const static int MAX_FORMAT_LENGTH = 8192;

        static std::string format(const char* format, ...);
        static std::string format(const char* format, std::va_list ap);
        static std::string trim(const std::string& s);

        static bool checkIntegerIdentialToString(const std::string& s, long long x);
        static bool checkDoubleIdentialToString(const std::string& s, double x);
        static std::string integerToString(long long x);
        static std::string doubleToString(double x);
        static int parseInt(const std::string& s);
        static double parseDouble(const std::string& s);
        static long long parseInt64(const std::string& s);
    };
}

#endif
