#include "InvalidCharacterEncoding.h"

InvalidCharacterEncoding::InvalidCharacterEncoding(const Token &token) : Error(token, "invalid character encoding")
{
}