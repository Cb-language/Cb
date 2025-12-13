#include "Utils.h"

#include <codecvt>
#include <locale>

SymbolTable* Utils::symTable = nullptr;

void Utils::init(SymbolTable* symTable)
{
    Utils::symTable = symTable;
}

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

void Utils::reset()
{
    symTable = nullptr;
}
