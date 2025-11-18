//
// Created by USER on 11/15/2025.
//

#include "ConstValueExpr.h"


ConstValueExpr::ConstValueExpr(const int line, const int column, const Type &type, const std::string &value) : Expr(line, column), type(type), value(value) {
}

bool ConstValueExpr::isLegal() const {
    if (value.empty()) {
        return false;
    }
    // literal is always legal
    return true;
}

std::string ConstValueExpr::translateToCpp() const {
    if (isLegal())
    {
        // casting wstring to string
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        std::string cppValue = conv.to_bytes(value);

        return cppValue;

    }
    else
    {
        return "/* illegal_const */ 0";
    }
}

Type ConstValueExpr::getType() const {
    return type;
}
