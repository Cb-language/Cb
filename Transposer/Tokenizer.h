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

public:
	static std::vector<Token> tokenize(const std::wstring& code);
};