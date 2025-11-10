#include "Token.h"

Token::Token(const TokenType type, const std::wstring &value, const size_t line, const size_t column)
	: type(type), value(value), line(line), column(column)
{
}
