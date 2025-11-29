#include "MissingSemicolon.h"

MissingSemicolon::MissingSemicolon(const Token &token) : Error(token, "missing semicolon")
{
}
