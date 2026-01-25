#include "token/Token.h"

Token::Token(const TokenType type, const std::wstring &value, const size_t line, const size_t column, const std::filesystem::path& path)
	: type(type), value(value), line(line), column(column), path(path)
{
}

bool Token::isConst() const
{
	return type == Token::CONST_STR || type == Token::CONST_CHAR || type == CONST_BOOL || type == Token::CONST_INT || type == Token::CONST_FLOAT;
}
