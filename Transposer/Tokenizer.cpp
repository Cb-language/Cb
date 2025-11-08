#include "Tokenizer.h"

boost::wregex Tokenizer::token_regex(
    LR"(

    )",
    boost::regex::perl | boost::regex::optimize | boost::regex::mod_x | boost::regex::mod_s
);

std::vector<Token> Tokenizer::tokenize(const std::wstring& code)
{
    return std::vector<Token>();
}
