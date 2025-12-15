#include "ElseIfWithoutIf.h"

ElseIfWithoutIf::ElseIfWithoutIf(const Token &token) : Error(token, "else if without if statement before it")
{
}
