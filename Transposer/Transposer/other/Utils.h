#pragma once
#include <filesystem>
#include <string>

class Utils
{
public:
    static std::string wstrToStr(const std::wstring& wstr);

    static void logMsg(const std::string& msg);

    static std::string removeFirstTabs(std::string& str);

    static std::string normalizePath(const std::filesystem::path& path);

    static bool startsWithNote(const std::wstring& wstr);
};
