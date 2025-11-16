//
// Created by USER on 11/15/2025.
//

#include "ConstValueExpr.h"

ConstValueExpr::ConstValueExpr(const int line, const int column, const Type &type, const std::string &value) : Expression(line, column), type(type), value(value) {
}

Type ConstValueExpr::getType() const {
    return type;
}
bool ConstValueExpr::isLegal() const {
    // literal is always legal
    return true;
}

std::string ConstValueExpr::print() const {

}
