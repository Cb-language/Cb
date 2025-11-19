//
// Created by USER on 11/15/2025.
//

#include "ConstValueExpr.h"


ConstValueExpr::ConstValueExpr(const Type &type, const std::wstring &value) : type(type), value(value) {
}

bool ConstValueExpr::isLegal() const {
    if (value.empty()) {
        return false;
    }
    // literal is always legal
    return true;
}

std::string ConstValueExpr::translateToCpp() const {
    if (type.getType() == L"freq" && value.starts_with(L"."))
    {
        return Utils::wstrToStr(L"0" + value);
    }

    return Utils::wstrToStr(value);
}

Type ConstValueExpr::getType() const {
    return type;
}
