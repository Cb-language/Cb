#include "ElseWithoutIf.h"

ElseWithoutIf::ElseWithoutIf(const Token &token) : Error(token, "else if without if statement before it")
{
}
