#include "InvalidNumberLiteral.h"

InvalidNumberLiteral::InvalidNumberLiteral(const int line, const int column, const std::string &token) : Error(line, column, "invalid number literal", token)
{
}