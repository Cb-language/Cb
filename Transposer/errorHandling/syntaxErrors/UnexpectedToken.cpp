#include "UnexpectedToken.h"

UnexpectedToken::UnexpectedToken(const Token &token) : Error(token, "unexpected token")
{
}