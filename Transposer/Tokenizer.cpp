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
    LR"((?<Type>\b(((flat|sharp)[\s\r\n]*)?(degree|freq|note))|(mute|bar|scale|fermata))\b))",
    LR"((?<Keyword>\b(pause|break|play(Bar)?|hear|D|E|A|C|B|G|Fmin|Fmaj|©)\b))",
    LR"((?<UnaryOp>)♯|♭|♮)",
    LR"((?<AssignmentOp>\+=|-=|//=|/=|\*=|%=))",
    LR"((?<BinaryOp>==|!=|>=|<=|<|>|\+|-|//|/|\*|%|\b(divis|chord)\b))",
    LR"((?<EqSign>=))",
    LR"((?<NotSign>!))",
    LR"((?<Punctuation>[𝄞𝄌\[\]║\\]))",
    LR"((?<Identifier>\b[a-zA-Z_]\w*\b))",
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
        else if (match[L"ConstStr"].matched) 
        {
            std::wstring txt = match.str();
            tokens.push_back(Token(CONST_STR, txt, current_line, current_col)); 

            size_t newlines = std::count(txt.begin(), txt.end(), L'\n');

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

            tokens.push_back(Token(TYPE, match.str(), current_line, current_col));
            size_t newlines = std::count(typeText.begin(), typeText.end(), L'\n');

            if (newlines > 0)
            {
                current_col = 0;
                current_line += newlines;
            }
        }
        else if (match[L"Keyword"].matched) { tokens.push_back(Token(KEYWORD, match.str(), current_line, current_col)); }
        else if (match[L"UnaryOp"].matched || match[L"NotSign"].matched) { tokens.push_back(Token(OP_UNARY, match.str(), current_line, current_col)); }
        else if (match[L"AssignmentOp"].matched || match[L"EqSign"].matched) { tokens.push_back(Token(OP_ASSIGNMENT, match.str(), current_line, current_col)); }
        else if (match[L"BinaryOp"].matched) { tokens.push_back(Token(OP_BINARY, match.str(), current_line, current_col)); }
        else if (match[L"Punctuation"].matched) { tokens.push_back(Token(PUNCTUATION, match.str(), current_line, current_col)); }
        else if (match[L"Identifier"].matched) { tokens.push_back(Token(IDENTIFIAR, match.str(), current_line, current_col)); }

        current_col++;
    }

    return tokens;
}
