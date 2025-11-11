#include "UnexpectedToken.h"

UnexpectedToken::UnexpectedToken(const int line, const int column, const std::string &token) : Error(line, column, "unexpected token", token)
{
}