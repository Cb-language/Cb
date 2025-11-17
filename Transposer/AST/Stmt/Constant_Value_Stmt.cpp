//
// Created by Cyber_User on 11/17/2025.
//

#include "Constant_Value_Stmt.h"



Constant_Value_Stmt::Constant_Value_Stmt(const std::wstring& value) : value(value) {}

bool Constant_Value_Stmt::isLegal() const
{
    if (value.empty()) {
        return false;
    }
    // literal value always legal
    return true;
}

std::string Constant_Value_Stmt::translateToCpp() const
{
    if (isLegal())
    {
        // casting wstring to string
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        std::string cppValue = conv.to_bytes(value);
        return cppValue;
    }
    else
    {
        return "Invalid Argument";
    }
}

std::wstring Constant_Value_Stmt::getValue() const
{
    return value;
}
