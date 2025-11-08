#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/regex.hpp>
#include "Token.h"

class Tokenizer
{
private:
	static boost::wregex token_regex;
	static const std::vector<std::wstring> capture_blocks;
	static bool inited;

public:
	static void init();
	static std::vector<Token> tokenize(const std::wstring& code);
};