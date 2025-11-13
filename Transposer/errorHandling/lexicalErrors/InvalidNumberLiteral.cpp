#include "InvalidNumberLiteral.h"

InvalidNumberLiteral::InvalidNumberLiteral(const Token &token) : Error(token, "invalid number literal")
{
}