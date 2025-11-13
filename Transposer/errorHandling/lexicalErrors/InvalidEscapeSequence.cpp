#include "InvalidEscapeSequence.h"

InvalidEscapeSequence::InvalidEscapeSequence(const Token &token) : Error(token, "invalid escape sequence")
{
}
