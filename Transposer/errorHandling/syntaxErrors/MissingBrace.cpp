#include "MissingBrace.h"

MissingBrace::MissingBrace(const int line, const int column, const std::string &token): Error(line, column, "missing brace", token)
{
}