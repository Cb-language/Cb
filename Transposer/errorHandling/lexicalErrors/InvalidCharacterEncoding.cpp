#include "InvalidCharacterEncoding.h"

InvalidCharacterEncoding::InvalidCharacterEncoding(const int line, const int column, const std::string& token) : Error(line, column, "invalid character encoding", token)
{
}