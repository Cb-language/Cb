#include "UnterminatedString.h"

UnterminatedString::UnterminatedString(const Token &token) : Error(token, "unterminated string")
{
}