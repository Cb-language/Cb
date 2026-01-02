#include "InvalidNumberLiteral.h"

InvalidNumberLiteral::InvalidNumberLiteral(const Token& token) : Error(token, "Invalid number literal")
{
}
