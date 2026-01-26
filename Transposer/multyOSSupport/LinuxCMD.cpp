#include "LinuxCMD.h"

void LinuxCMD::setupConsole()
{
    // No setup needed for Linux typically
}

std::string LinuxCMD::getNullDevice() const
{
    return ">/dev/null 2>&1";
}

std::string LinuxCMD::getExeExtension() const

{
    return "";
}

std::string LinuxCMD::getCompileFlags() const
{
    return ""; // No specific flags needed
}

void LinuxCMD::runExecutable(const std::string& path) const
{
    std::string cmd;
    if (path.find('/') == 0)
    {
        cmd = "\"" + path + "\"";
    }
    else
    {
        cmd = "./\"" + path + "\"";
    }
    std::system(cmd.c_str());
}

std::unique_ptr<CMD> createCMD()
{
    return std::make_unique<LinuxCMD>();
}
