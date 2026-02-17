#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <codecvt>
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

std::string Utils::normalizePath(const std::filesystem::path& path)
{
    std::string res;
    bool dontAddBackslash = true;
    for (auto& part : path)
    {
        if (!dontAddBackslash)
        {
            if (part == "\\") continue;
            res += "/";
        }
        else dontAddBackslash = false;
        res += part.string();
    }
    return res;
}

bool Utils::startsWithNote(const std::wstring& wstr)
{
    return wstr.starts_with(L"do_") || wstr.starts_with(L"re_") || wstr.starts_with(L"mi_") ||
            wstr.starts_with(L"fa_") || wstr.starts_with(L"sol_") || wstr.starts_with(L"la_") ||
            wstr.starts_with(L"si_") || wstr.starts_with(L"ti_");
}

std::string Utils::escapeJson(const std::string& str)
{
    std::string res;
    for (const char c : str)
    {
        switch (c)
        {
            case '"':
                res += "\\\"";
                break;
            case '\\':
                res += "\\\\";
                break;
            default:
                res += c;
        }
    }
    return res;
}
