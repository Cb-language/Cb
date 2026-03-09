#include "NoNewLine.h"

NoNewLine::NoNewLine(const Token& token) : Error(token, "expected a new line")
{
}
