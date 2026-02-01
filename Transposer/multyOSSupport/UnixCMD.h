#pragma once
#ifndef _WIN32

#include "CMD.h"

class UnixCMD : public CMD
{
public:
    void setupConsole() override;
    std::string getNullDevice() const override;
    std::string getExeExtension() const override;
    std::string getCompileFlags() const override;
    void runExecutable(const std::string& path) const override;
};

#endif