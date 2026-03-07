#include "Tokenizer.h"

#include "TrieTree/Keywords.h"

boost::wregex Tokenizer::tokenRegex;

const std::vector<std::wstring> Tokenizer::captureBlocks = {
    LR"((?<IdentifierCall>[a-zA-Z_][a-zA-Z_0-9]*_call))",
    LR"((?<Identifier>[a-zA-Z_][a-zA-Z_0-9]*))"
};

void Tokenizer::initTrieTree() const
{
    for (const auto& keyword : KEYWORDS)
    {
        auto current = trieTree.get();

        for (const auto& c : keyword.keyword)
        {
            current = &current->getOrCreateCild(c);
        }
        current->setKeyword(keyword);
    }
}

std::vector<Token> Tokenizer::tokenizeByTrieTree(const std::wstring& code, const std::filesystem::path& path)
{
    std::vector<Token> tokens;
    size_t current_line = 1;
    size_t current_col = 1;
    int i = 0;

    while (i < code.size())
    {
        current_col++;

        if (code[i] == " ")
    }
}

Tokenizer::Tokenizer()
{
    initTrieTree();

    std::wstring regex = LR"()";

    bool first = true;
    for (auto& block : captureBlocks)
    {
        if (!first)
        {
            regex += LR"(|)";
        }
        regex += block;
        first = false;
    }

    tokenRegex = boost::wregex(regex, boost::regex::perl | boost::regex::optimize);
}

std::vector<Token> Tokenizer::tokenize(const std::wstring& code, const std::filesystem::path& path)
{
    std::vector<Token> tokens;
    size_t current_line = 1;
    size_t current_col = 1;

    boost::wsregex_iterator it(code.begin(), code.end(), tokenRegex);
    boost::wsregex_iterator end;

    for (; it != end; ++it)
    {
        const boost::wsmatch& match = *it;

        if (match[L"Newline"].matched) { current_line++; current_col = 0; }
        else if (match[L"CommentSingle"].matched) { tokens.push_back(Token(TokenType::COMMENT_SINGLE, match.str(), current_line, current_col, path)); }
        else if (match[L"CommentMulti"].matched) { tokens.push_back(Token(TokenType::COMMENT_MULTI, match.str(), current_line, current_col, path)); }
        else if (match[L"ConstBool"].matched) { tokens.push_back(Token(TokenType::CONST_BOOL, match.str(), current_line, current_col, path)); }
        else if (match[L"ConstFloat"].matched) { tokens.push_back(Token(TokenType::CONST_FLOAT, match.str(), current_line, current_col, path)); }
        else if (match[L"ConstInt"].matched) { tokens.push_back(Token(TokenType::CONST_INT, match.str(), current_line, current_col, path)); }
        else if (match[L"ConstChar"].matched) { tokens.push_back(Token(TokenType::CONST_CHAR, match.str(), current_line, current_col, path)); }
        else if (match[L"ConstStr"].matched) 
        {
            std::wstring txt = match.str();
            tokens.push_back(Token(TokenType::CONST_STR, txt, current_line, current_col, path));

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

            tokens.push_back(Token(TokenType::TYPE, match.str(), current_line, current_col, path));
            size_t newlines = std::ranges::count(typeText, L'\n');

            if (newlines > 0)
            {
                current_col = 0;
                current_line += newlines;
            }
        }
        else if (match[L"Keyword"].matched) { tokens.push_back(Token(TokenType::KEYWORD, match.str(), current_line, current_col, path)); }
        else if (match[L"UnaryOp"].matched) { tokens.push_back(Token(TokenType::OP_UNARY, match.str(), current_line, current_col, path)); }
        else if (match[L"AssignmentOp"].matched) { tokens.push_back(Token(TokenType::OP_ASSIGNMENT, match.str(), current_line, current_col, path)); }
        else if (match[L"BinaryOp"].matched) { tokens.push_back(Token(TokenType::OP_BINARY, match.str(), current_line, current_col, path)); }
        else if (match[L"Punctuation"].matched) { tokens.push_back(Token(TokenType::PUNCTUATION, match.str(), current_line, current_col, path)); }
        else if (match[L"IdentifierCall"].matched) { tokens.push_back(Token(TokenType::IDENTIFIER_CALL, match.str(), current_line, current_col, path)); }
        else if (match[L"Identifier"].matched) { tokens.push_back(Token(TokenType::IDENTIFIER, match.str(), current_line, current_col, path)); }

        current_col++;
    }

    return clean(tokens);
}
