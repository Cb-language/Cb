#include "IncludeNotInTop.h"

IncludeNotInTop::IncludeNotInTop(const Token &token) : Error(token, "include statement must be at the top of the file")
{
}