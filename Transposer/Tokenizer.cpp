#include "Tokenizer.h"

boost::wregex Tokenizer::token_regex;

const std::vector<std::wstring> Tokenizer::capture_blocks = {
    LR"((?<CommentSingle>\?\s?[^\n]*))", // Single-line comment
    LR"((?<CommentMulti>\?\*[\s\S]*?\*\?))" // Multi-Line comment
};

bool Tokenizer::inited = false;

void Tokenizer::init()
{
    if (inited)
    {
        return;
    }

    std::wstring regex = LR"()";

    bool first = true;
    for (auto& block : capture_blocks)
    {
        if (!first)
        {
            regex += LR"( | )";
        }
        regex += block;
        first = false;
    }

    token_regex = boost::wregex(regex, boost::regex::perl | boost::regex::optimize);

    inited = true;
}

std::vector<Token> Tokenizer::tokenize(const std::wstring& code)
{
    std::vector<Token> tokens;
    size_t current_line = 1;
    size_t current_col = 1;

    return tokens;
}
