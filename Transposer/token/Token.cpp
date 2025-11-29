#include "token/Token.h"

Token::Token(const TokenType type, const std::wstring &value, const size_t line, const size_t column)
	: type(type), value(value), line(line), column(column)
{
}

bool Token::isConst() const
{
	return type == Token::CONST_STR || type == Token::CONST_CHAR || type == CONST_BOOL || type == Token::CONST_INT || type == Token::CONST_FLOAT;
}
