#pragma once
#include <filesystem>
#include <iostream>
#include <string>

typedef unsigned char byte;

struct Token
{
	enum TokenType : byte
	{
		UNDEFINED_TOKEN = 0,
		IDENTIFIER = 1,
		IDENTIFIER_CALL,
		TYPE,
		KEYWORD,
		PUNCTUATION,
		OP_UNARY,
		OP_BINARY,
		OP_ASSIGNMENT,

		CONST_INT = 10,
		CONST_FLOAT,
		CONST_CHAR,
		CONST_BOOL,
		CONST_STR,

		COMMENT_SINGLE = 20,
		COMMENT_MULTI
	};

	TokenType type;
	std::wstring value;

	size_t line;
	size_t column;

	std::filesystem::path path;

	Token();

	Token(TokenType type, const std::wstring &value, size_t line, size_t column, const std::filesystem::path& path);

	bool isConst() const;
};