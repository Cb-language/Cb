#pragma once

#ifdef _WIN32 //windows case

#include <windows.h>
#include "CMD.h"

class WindowsCMD : public CMD
{
public:
    
    void setupConsole() override
    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);

        SetConsoleOutputCP(CP_UTF8);
    }

    std::string getNullDevice() const override
    {
        return ">nul 2>&1";
    }

    std::string getExeExtension() const override
    {
        return ".exe";
    }

    std::string getCompileFlags() const override
    {
        return " -static -static-libgcc -static-libstdc++";
    }

    void runExecutable(const std::string& path) const override
    {
        // Windows: Open new window and pause
        std::string cmd = "start \"\" cmd /c \"" + path + " & pause\"";
        std::system(cmd.c_str());
    }

};
inline std::unique_ptr<CMD> createCMD()
{
    return std::make_unique<WindowsCMD>();
}

#else // unix case

#include <cstdlib>
#include <memory>
#include <string>

#include "CMD.h"

class UnixCMD : public CMD
{
public:

    void setupConsole() override
    {
        // No setup needed for Linux typically
    }

    std::string getNullDevice() const override
    {
        return ">/dev/null 2>&1";
    }

    std::string getExeExtension() const override

    {
        return "";
    }

    std::string getCompileFlags() const override
    {
        return ""; // No specific flags needed
    }

    void runExecutable(const std::string& path) const override
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


};

inline std::unique_ptr<CMD> createCMD()
{
    return std::make_unique<UnixCMD>();
}
#endif