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

std::string Utils::printTabs()
{
    const int level = symTable->getLevel() + 1; // currently everything is in the main
    std::string res;

    for (auto i = 0; i < level; i++)
    {
        res += "\t";
    }

    return res;
}

void Utils::reset()
{
    symTable = nullptr;
}
