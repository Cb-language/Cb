#include "InvalidExpression.h"

InvalidExpression::InvalidExpression(const int line, const int column, const std::string &token) : Error(line, column, "invalid expression", token) {
}