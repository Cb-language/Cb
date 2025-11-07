#pragma once
#include <iostream>
#include <string>

typedef unsigned char byte;

enum TokenType : byte
{
	IDENTIFIAR = 1,
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

struct Token
{
	const TokenType type;
	const std::wstring value;

	Token(const TokenType type, const std::wstring value);
};