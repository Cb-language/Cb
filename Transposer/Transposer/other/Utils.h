#pragma once
#include <filesystem>
#include <string>
#include <bits/stl_vector.h>

class Utils
{
public:
    static std::string wstrToStr(const std::wstring& wstr);

    static void logMsg(const std::string& msg);

    static std::string removeFirstTabs(std::string& str);

    static std::string normalizePath(const std::filesystem::path& path);

    static bool startsWithNote(const std::wstring& wstr);
    static std::string escapeJson(const std::string& str);

    static std::string getAllObjIncludes();
    static std::vector<std::filesystem::path> getAllObjCppPaths();
};
