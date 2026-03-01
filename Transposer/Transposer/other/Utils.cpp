#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <codecvt>
#include <locale>
#include <vector>

#include "files/File.h"

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

std::string Utils::removeFirstTabs(std::string str)
{
    if (!str.starts_with("\t"))
    {
        return str;
    }
    str.erase(0, 1);
    return removeFirstTabs(str);
}

std::string Utils::removeAllFirstTabs(std::string str)
{
    bool atLineStart = true;

    for (size_t i = 0; i < str.size(); )
    {
        if (atLineStart && str[i] == '\t')
        {
            str.erase(i, 1);
            atLineStart = false;
            continue;
        }

        if (str[i] == '\n')
        {
            atLineStart = true;
        }
        else
        {
            atLineStart = false;
        }

        ++i;
    }

    return str;
}

std::string Utils::normalizePath(const std::filesystem::path& path)
{
    return path.generic_string();
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

std::string Utils::getAllObjIncludes()
{
    std::ostringstream oss;

    oss << R"(#include "includes/Object.h")" << std::endl <<
        R"(#include "includes/String.h")" << std::endl <<
        R"(#include "includes/Array.h")" << std::endl <<
        R"(#include "includes/Primitive.h")" << std::endl <<
        R"(#include "includes/SafePtr.h")" << std::endl;

    return oss.str();
}

std::vector<std::filesystem::path> Utils::getAllObjCppPaths()
{
    const std::filesystem::path out = File::getOutDir() / "includes";
    std::vector<std::filesystem::path> p = {
        out / "Object.cpp",
        out / "String.cpp"
    };

    return p;
}
