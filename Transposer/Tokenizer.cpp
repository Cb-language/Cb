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

    boost::wsregex_iterator it(code.begin(), code.end(), token_regex);
    boost::wsregex_iterator end;

    for (; it != end; ++it)
    {
        const boost::wsmatch& match = *it;

        if (match[L"CommentSingle"].matched) { tokens.push_back(Token(COMMENT_SINGLE, match.str(), current_line, current_col)); }
        else if (match[L"CommentMulti"].matched) { tokens.push_back(Token(COMMENT_MULTI, match.str(), current_line, current_col)); }

        current_col++;
    }

    return tokens;
}
