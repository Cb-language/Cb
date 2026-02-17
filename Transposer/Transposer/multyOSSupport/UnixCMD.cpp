#ifndef _WIN32

#include "UnixCMD.h"
#include <cstdlib>

void UnixCMD::setupConsole()
{
    // No setup needed
}

std::string UnixCMD::getNullDevice() const
{
    return ">/dev/null 2>&1";
}

std::string UnixCMD::getExeExtension() const
{
    return "";
}

std::string UnixCMD::getCompileFlags() const
{
    return "";
}

void UnixCMD::runExecutable(const std::string& path) const
{
    std::string cmd =
        (path.front() == '/') ? "\"" + path + "\"" : "./\"" + path + "\"";
    std::system(cmd.c_str());
}

#endif