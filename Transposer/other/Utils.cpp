#include "Utils.h"

#include <codecvt>
#include <iostream>
#include <locale>

std::string Utils::wstrToStr(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

void Utils::logMsg(const std::string& msg)
{
    std::cout << "\033[36m"   // cyan
        << msg
        << "\033[0m" // reset
        << std::endl;
}

std::string Utils::removeFirstTabs(std::string& str)
{
    if (!str.starts_with("\t"))
    {
        return str;
    }
    str.erase(0, 1);
    return removeFirstTabs(str);
}
