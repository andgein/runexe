#define _CRT_SECURE_NO_DEPRECATE

#include "Strings.h"
#include "Run.h"

#include <string>
#include <cctype>
#include <cstdio>
#include <sstream>

using namespace runexe;
using namespace std;

string Strings::format(const char* format, va_list ap)
{
    char* msg = new char[MAX_FORMAT_LENGTH];    

    vsprintf(msg, format, ap);

    string result = msg;
    
    delete[] msg;

    return result;
}

string Strings::format(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    string result = Strings::format(format, ap);
    va_end(ap);
    return result;
}

string Strings::trim(const string& s)
{
    int first = 0, last = (int)s.length();

    while (first < last)
    {
        if ((unsigned char)s[first] <= ' ')
        {
            ++first;
            continue;
        }

        if ((unsigned char)s[last - 1] <= ' ')
        {
            --last;
            continue;
        }

        break;
    }

    return s.substr(first, last - first);
}

bool Strings::checkIntegerIdentialToString(const string& s, long long x)
{
    return s == integerToString(x);
}

bool Strings::checkDoubleIdentialToString(const string& s, double x)
{
    return s == doubleToString(x);
}

string Strings::integerToString(long long x)
{
    stringstream stream;
    stream << x;
    return stream.str();
}

string Strings::doubleToString(double x)
{
    stringstream stream;
    stream << x;
    return stream.str();
}

int Strings::parseInt(const string& s)
{
    int result = atoi(s.c_str());

    if (!checkIntegerIdentialToString(s, result))
        crash("expected int but received '" + s + "'");

    return result;
}

double Strings::parseDouble(const string& s)
{
    double result = atof(s.c_str());

    if (!checkDoubleIdentialToString(s, result))
        crash("expected double but received '" + s + "'");

    return result;
}

long long Strings::parseInt64(const string& s)
{
    long long result = _atoi64(s.c_str());

    if (!checkIntegerIdentialToString(s, result))
        crash("expected int64 but received '" + s + "'");

    return result;
}
