#include "MissingSemicolon.h"

MissingSemicolon::MissingSemicolon(const int line, const int column, const std::string &token) : Error(line, column, "missing semicolon", token)
{
}
