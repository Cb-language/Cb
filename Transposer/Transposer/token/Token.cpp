#include "token/Token.h"

Token::Token() : type(CbTokenType::ERROR_TOKEN), value(""), line(0), column(0), path("")
{
}

Token::Token(const CbTokenType type, const std::optional<std::string>& value, const size_t line, const size_t column,
	const std::filesystem::path& path)
: type(type), value(std::move(value)), line(line), column(column), path(path)
{
}

bool Token::isConst() const
{
	return type == CbTokenType::CONST_STR || type == CbTokenType::CONST_CHAR || type == CbTokenType::CONST_BOOL || type == CbTokenType::CONST_INT || type == CbTokenType::CONST_FLOAT;
}
