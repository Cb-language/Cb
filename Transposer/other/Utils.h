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

    static void logMsg(const std::string& msg);

    static void reset();
};
