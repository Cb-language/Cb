//
// Created by USER on 11/15/2025.
//

#include "ConstValueExpr.h"


ConstValueExpr::ConstValueExpr(const int line, const int column, const Type &type, const std::wstring &value) : Expr(line, column), type(type), value(value) {
}

bool ConstValueExpr::isLegal() const {
    if (value.empty()) {
        return false;
    }
    // literal is always legal
    return true;
}

std::string ConstValueExpr::translateToCpp() const {
    return type.translateTypeToCpp();
}

Type ConstValueExpr::getType() const {
    return type;
}
