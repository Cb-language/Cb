#include "token/Token.h"

Token::Token() : type(TokenType::ERROR_TOKEN), value(""), line(0), column(0), path("")
{
}

Token::Token(const TokenType type, const std::optional<std::string>& value, const size_t line, const size_t column,
	const std::filesystem::path& path)
: type(type), value(std::move(value)), line(line), column(column), path(path)
{
}

bool Token::isConst() const
{
	return type == TokenType::CONST_STR || type == TokenType::CONST_CHAR || type == TokenType::CONST_BOOL || type == TokenType::CONST_INT || type == TokenType::CONST_FLOAT;
}
