#pragma once
#include <string>

class Utils
{
public:
    static std::string wstrToStr(const std::wstring& wstr);

    static void logMsg(const std::string& msg);

    static std::string removeFirstTabs(std::string& str);
};
