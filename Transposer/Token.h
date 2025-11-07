#pragma once
#include <iostream>
#include <string>

typedef unsigned char byte;

enum TokenType : byte
{
};

struct Token
{
	const TokenType type;
	const std::wstring value;

	Token(const TokenType type, const std::wstring value);
};