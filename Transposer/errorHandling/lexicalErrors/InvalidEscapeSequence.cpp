#include "InvalidEscapeSequence.h"

InvalidEscapeSequence::InvalidEscapeSequence(const int line, const int column, const std::string &token) : Error(line, column, "invalid escape sequence", token)
{
}
