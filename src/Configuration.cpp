#include "Configuration.h"

#include <string>

using namespace runexe;
using namespace std;

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

Configuration& Configuration::getConfiguration()
{
    static Configuration configuration;
    static bool initialized = false;

    if (!initialized)
    {
        configuration.setDefaults();
        initialized = true;
    }

    return configuration;
}

void Configuration::setDefaults()
{
    setReturnExitCode(RETURN_EXIT_CODE);
    setScreenOutput(SCREEN_OUTPUT);
    setXmlOutput(XML_OUTPUT);
    setStatisticsFileName(STATISTICS_FILE_NAME);
    setXmlFileName(XML_FILE_NAME);
}

bool Configuration::isReturnExitCode() const
{
    return returnExitCode;
}

void Configuration::setReturnExitCode(bool returnExitCode)
{
    this->returnExitCode = returnExitCode;
}

bool Configuration::isScreenOutput() const
{
    return screenOutput;
}

void Configuration::setScreenOutput(bool screenOutput)
{
    this->screenOutput = screenOutput;
}

bool Configuration::isXmlOutput() const
{
    return xmlOutput;
}

void Configuration::setXmlOutput(bool xmlOutput)
{
    this->xmlOutput = xmlOutput;
}

string Configuration::getStatisticsFileName() const
{
    return statisticsFileName;
}

void Configuration::setStatisticsFileName(const string& statisticsFileName)
{
    this->statisticsFileName = statisticsFileName;
}

string Configuration::getXmlFileName() const
{
    return xmlFileName;
}

void Configuration::setXmlFileName(const std::string& xmlFileName)
{
    this->xmlFileName = xmlFileName;
}
