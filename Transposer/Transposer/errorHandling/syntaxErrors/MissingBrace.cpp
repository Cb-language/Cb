#include "MissingBrace.h"

MissingBrace::MissingBrace(const Token &token): Error(token, "missing brace")
{
}