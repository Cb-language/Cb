#include "Tokenizer.h"

boost::wregex Tokenizer::token_regex;

const std::vector<std::wstring> Tokenizer::capture_blocks = {
    LR"((?<CommentSingle>\?\s?[^\n]*))",
    LR"((?<CommentMulti>\?\*[\s\S]*?\*\?))",
    LR"((?<ConstBool>\b(cres|demen)\b))",
    LR"((?<ConstFloat>\d*\.\d+))",
    LR"((?<ConstInt>\d+))",
    LR"((?<ConstChar>'(\\.|[^\\'\n])'))",
    LR"((?<ConstStr>"(\\.|[^"\n])*"))",
    LR"((?<Type>\b(((flat|sharp)[\s\r\n]*)?(degree|freq|note))|(mute|bar|scale))\b))",

    LR"((?<Newline>\n))"
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
            regex += LR"(|)";
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

        if (match[L"Newline"].matched) { current_line++; current_col = 0; }
        else if (match[L"CommentSingle"].matched) { tokens.push_back(Token(COMMENT_SINGLE, match.str(), current_line, current_col)); }
        else if (match[L"CommentMulti"].matched) { tokens.push_back(Token(COMMENT_MULTI, match.str(), current_line, current_col)); }
        else if (match[L"ConstBool"].matched) { tokens.push_back(Token(CONST_BOOL, match.str(), current_line, current_col)); }
        else if (match[L"ConstFloat"].matched) { tokens.push_back(Token(CONST_FLOAT, match.str(), current_line, current_col)); }
        else if (match[L"ConstInt"].matched) { tokens.push_back(Token(CONST_INT, match.str(), current_line, current_col)); }
        else if (match[L"ConstChar"].matched) { tokens.push_back(Token(CONST_CHAR, match.str(), current_line, current_col)); }
        else if (match[L"ConstStr"].matched) { tokens.push_back(Token(CONST_STR, match.str(), current_line, current_col)); }

        current_col++;
    }

    return tokens;
}
