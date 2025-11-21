#pragma once
#include <string>

#include "SymbolTable.h"

class Utils
{
private:
    static SymbolTable* symTable;
public:
    static void init(SymbolTable* symTable);

    static std::string wstrToStr(const std::wstring& wstr);
    static std::string printTabs();

    static void reset();
};
