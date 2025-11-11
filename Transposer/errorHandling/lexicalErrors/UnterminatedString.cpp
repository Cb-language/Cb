#include "UnterminatedString.h"

UnterminatedString::UnterminatedString(const int line, const int column, const std::string &token) : Error(line, column, "unterminated string literal", token)
{
}