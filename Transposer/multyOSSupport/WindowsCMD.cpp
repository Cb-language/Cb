#ifdef _WIN32

#include "WindowsCMD.h"
#include <windows.h>
#include <cstdlib>

void WindowsCMD::setupConsole()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);

    SetConsoleOutputCP(CP_UTF8);
}

std::string WindowsCMD::getNullDevice() const
{
    return ">nul 2>&1";
}

std::string WindowsCMD::getExeExtension() const
{
    return ".exe";
}

std::string WindowsCMD::getCompileFlags() const
{
    return " -static -static-libgcc -static-libstdc++";
}

void WindowsCMD::runExecutable(const std::string& path) const
{
    std::string cmd = "start \"\" cmd /c \"" + path + " & pause\"";
    std::system(cmd.c_str());
}

#endif