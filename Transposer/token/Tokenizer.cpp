#include "Tokenizer.h"

boost::wregex Tokenizer::token_regex;

const std::vector<std::wstring> Tokenizer::capture_blocks = {
    LR"((?<CommentMulti>\?\*(.|\n)*?\*\?))",
    LR"((?<CommentSingle>\?\s?[^\n]*))",
    LR"((?<ConstBool>\b(cres|demen)\b))",
    LR"((?<ConstFloat>\d*\.\d+))",
    LR"((?<ConstInt>\d+))",
    LR"((?<ConstChar>'(\\.|[^\\'\n])'))",
    LR"((?<ConstStr>"(\\.|[\s\S])*?"))",
    LR"((?<Type>\b((flat|sharp)[\s\r\n]*(degree|freq|note))\b)|(?<Type>\b(degree|freq|note)\b)|(?<Type>\b(mute|bar|scale|fermata)\b))",
    LR"((?<Keyword>\b(pause|break|play(Bar)?|hear|D|E|A|C|B|G|Fmin|Fmaj|song)\b))",
    LR"((?<Punctuation>->))",
    LR"((?<UnaryOp>♯|♭|♮))",
    LR"((?<AssignmentOp>\+=|-=|//=|/=|\*=|%=))",
    LR"((?<BinaryOp>==|!=|>=|<=|<|>|\+|-|//|/|\*|%|\b(divis|chord)\b))",
    LR"((?<AssignmentOp>=))",
    LR"((?<UnaryOp>!))",
    LR"((?<Punctuation>(,|∮|☉|©|\[|\]|║\:|\:║|║|\\|\(|\))))",
    LR"((?<Identifier>[a-zA-Z_][a-zA-Z_0-9]*))",
    LR"((?<Newline>\n))"
};

bool Tokenizer::inited = false;

std::vector<Token> Tokenizer::clean(std::vector<Token>& tokens)
{
    std::vector<Token> cleaned;

    for (auto& token : tokens)
    {
        if (token.type != Token::COMMENT_SINGLE && token.type != Token::COMMENT_MULTI)
        {
            cleaned.push_back(token);
        }
    }

    return cleaned;
}

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
        else if (match[L"CommentSingle"].matched) { tokens.push_back(Token(Token::COMMENT_SINGLE, match.str(), current_line, current_col)); }
        else if (match[L"CommentMulti"].matched) { tokens.push_back(Token(Token::COMMENT_MULTI, match.str(), current_line, current_col)); }
        else if (match[L"ConstBool"].matched) { tokens.push_back(Token(Token::CONST_BOOL, match.str(), current_line, current_col)); }
        else if (match[L"ConstFloat"].matched) { tokens.push_back(Token(Token::CONST_FLOAT, match.str(), current_line, current_col)); }
        else if (match[L"ConstInt"].matched) { tokens.push_back(Token(Token::CONST_INT, match.str(), current_line, current_col)); }
        else if (match[L"ConstChar"].matched) { tokens.push_back(Token(Token::CONST_CHAR, match.str(), current_line, current_col)); }
        else if (match[L"ConstStr"].matched) 
        {
            std::wstring txt = match.str();
            tokens.push_back(Token(Token::CONST_STR, txt, current_line, current_col));

            size_t newlines = std::ranges::count(txt, L'\n');

            if (newlines > 0)
            {
                current_col = 0;
                current_line += newlines;
            }
        }
        else if (match[L"Type"].matched) 
        {
            std::wstring typeText = match.str();

            // Remove all extra whitespace/newlines between words
            std::wstring cleanedType;
            bool lastWasSpace = false;
            for (wchar_t ch : typeText) 
            {
                if (iswspace(ch)) 
                {
                    if (!lastWasSpace) 
                    {
                        cleanedType += L' '; // replace any whitespace sequence with single space
                        lastWasSpace = true;
                    }
                }
                else 
                {
                    cleanedType += ch;
                    lastWasSpace = false;
                }
            }

            // trim leading/trailing space
            if (!cleanedType.empty() && cleanedType.front() == L' ')
            {
                cleanedType.erase(0, 1);
            }
            if (!cleanedType.empty() && cleanedType.back() == L' ')
            {
                cleanedType.pop_back();
            }

            tokens.push_back(Token(Token::TYPE, match.str(), current_line, current_col));
            size_t newlines = std::ranges::count(typeText, L'\n');

            if (newlines > 0)
            {
                current_col = 0;
                current_line += newlines;
            }
        }
        else if (match[L"Keyword"].matched) { tokens.push_back(Token(Token::KEYWORD, match.str(), current_line, current_col)); }
        else if (match[L"UnaryOp"].matched) { tokens.push_back(Token(Token::OP_UNARY, match.str(), current_line, current_col)); }
        else if (match[L"AssignmentOp"].matched) { tokens.push_back(Token(Token::OP_ASSIGNMENT, match.str(), current_line, current_col)); }
        else if (match[L"BinaryOp"].matched) { tokens.push_back(Token(Token::OP_BINARY, match.str(), current_line, current_col)); }
        else if (match[L"Punctuation"].matched) { tokens.push_back(Token(Token::PUNCTUATION, match.str(), current_line, current_col)); }
        else if (match[L"Identifier"].matched) { tokens.push_back(Token(Token::IDENTIFIER, match.str(), current_line, current_col)); }

        current_col++;
    }

    return clean(tokens);
}
