#include "Tokenizer.h"

#include "TrieTree/Keywords.h"

boost::regex Tokenizer::tokenRegex;

const std::vector<std::string> Tokenizer::captureBlocks = {
    R"((?<IdentifierCall>[a-zA-Z_][a-zA-Z_0-9]*_call))",
    R"((?<Identifier>[a-zA-Z_][a-zA-Z_0-9]*))"
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

std::vector<Token> Tokenizer::tokenizeByTrieTree(const std::string& code, const std::filesystem::path& path)
{
    std::vector<Token> tokens;
    size_t row = 1;
    size_t col = 0;
    int codePos = 0;

    while (codePos < code.size())
    {
        col++;

        if (code[codePos] == ' ')
        {
            codePos++;
            continue;
        }

        const TrieNode* current = trieTree.get();
        const KeywordInfo* lastMatch = nullptr;
        size_t lastMatchEnd = codePos;

        for (size_t i = codePos; i < code.size() && current->getChild(code[i]); i++)
        {
            current = &current->getChild(code[i])->get();

            if (const auto keyword = current->getKeyword())
            {
                lastMatch = &keyword->get();
                lastMatchEnd = i + 1;
            }

            while (code[i] == ' ')
            {
                i++;
            }
        }

        if (lastMatch && checkBoundary(code, lastMatch, codePos, lastMatchEnd))
        {
            codePos = handleKeywordMatch(code, row, col, lastMatch->type, tokens, lastMatchEnd, path);
            continue;
        }


    }
    return tokens;
}

bool Tokenizer::checkBoundary(const std::string& code, const KeywordInfo* keyword, const size_t start, const size_t end)
{
    if (!keyword)
        return false;

    // If there are no boundaries at all, the checks don't really matter.
    if (keyword->boundary == WordBoundary::NONE)
        return true;


    if (keyword->boundary == WordBoundary::BEFORE || keyword->boundary == WordBoundary::BOTH)
    {
        // Check whether a character exists before this keyword
        if (start > 0 && isalnum(code[start-1]))
            return false;
    }

    if (keyword->boundary == WordBoundary::AFTER || keyword->boundary == WordBoundary::BOTH)
    {
        // Check whether a character exists after this keyword
        if (end < code.size() && isalnum(code[end]))
            return false;
    }


    return true;
}

size_t Tokenizer::handleKeywordMatch(const std::string& code, const size_t row, const size_t col, const TokenType tokenType,
    std::vector<Token>& tokens, const size_t keywordEnd, const std::filesystem::path& path)
{
    switch (tokenType)
    {
        case TokenType::COMMENT_SINGLE:
        {
            // If it's a one-line comment, look for the end of the line.
            const size_t end = code.find('\n', keywordEnd);

            if (end != std::string::npos) return end;

            return keywordEnd;
        }

        case TokenType::COMMENT_MULTI_START:
        {
            // If it's a multiline comment, look for a COMMENT_MULTY_END match.
            const size_t end = code.find(COMMENT_MULTY_END, keywordEnd);

            if (end != std::string::npos)
                return end + COMMENT_MULTY_END.length();

            tokens.emplace_back(TokenType::ERROR_TOKEN, std::nullopt, row, col, path);
        }

        default:
        {
            tokens.emplace_back(tokenType, std::nullopt, row, col, path);
            return keywordEnd;
        }
    }
}

Tokenizer::Tokenizer()
{
    initTrieTree();

    std::string regex = R"()";

    bool first = true;
    for (auto& block : captureBlocks)
    {
        if (!first)
        {
            regex += R"(|)";
        }
        regex += block;
        first = false;
    }

    tokenRegex = boost::regex(regex, boost::regex::perl | boost::regex::optimize);
}

std::vector<Token> Tokenizer::tokenize(const std::string& code, const std::filesystem::path& path)
{
    std::vector<Token> tokens;
    size_t current_line = 1;
    size_t current_col = 1;

    boost::sregex_iterator it(code.begin(), code.end(), tokenRegex);
    boost::sregex_iterator end;

    for (; it != end; ++it)
    {
        const boost::smatch& match = *it;

        if (match["Newline"].matched) { current_line++; current_col = 0; }
        else if (match["CommentSingle"].matched) { tokens.push_back(Token(TokenType::COMMENT_SINGLE, match.str(), current_line, current_col, path)); }
        else if (match["CommentMulti"].matched) { tokens.push_back(Token(TokenType::COMMENT_MULTI, match.str(), current_line, current_col, path)); }
        else if (match["ConstBoo"].matched) { tokens.push_back(Token(TokenType::CONST_BOOL, match.str(), current_line, current_col, path)); }
        else if (match["ConstFloat"].matched) { tokens.push_back(Token(TokenType::CONST_FLOAT, match.str(), current_line, current_col, path)); }
        else if (match["ConstInt"].matched) { tokens.push_back(Token(TokenType::CONST_INT, match.str(), current_line, current_col, path)); }
        else if (match["ConstChar"].matched) { tokens.push_back(Token(TokenType::CONST_CHAR, match.str(), current_line, current_col, path)); }
        else if (match["ConstStr"].matched)
        {
            std::string txt = match.str();
            tokens.push_back(Token(TokenType::CONST_STR, txt, current_line, current_col, path));

            size_t newlines = std::ranges::count(txt, L'\n');

            if (newlines > 0)
            {
                current_col = 0;
                current_line += newlines;
            }
        }
        else if (match["Type"].matched)
        {
            std::string typeText = match.str();

            // Remove all extra whitespace/newlines between words
            std::string cleanedType;
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
        else if (match["Keyword"].matched) { tokens.push_back(Token(TokenType::KEYWORD, match.str(), current_line, current_col, path)); }
        else if (match["UnaryOp"].matched) { tokens.push_back(Token(TokenType::OP_UNARY, match.str(), current_line, current_col, path)); }
        else if (match["AssignmentOp"].matched) { tokens.push_back(Token(TokenType::OP_ASSIGNMENT, match.str(), current_line, current_col, path)); }
        else if (match["BinaryOp"].matched) { tokens.push_back(Token(TokenType::OP_BINARY, match.str(), current_line, current_col, path)); }
        else if (match["Punctuation"].matched) { tokens.push_back(Token(TokenType::PUNCTUATION, match.str(), current_line, current_col, path)); }
        else if (match["IdentifierCal"].matched) { tokens.push_back(Token(TokenType::IDENTIFIER_CALL, match.str(), current_line, current_col, path)); }
        else if (match["Identifier"].matched) { tokens.push_back(Token(TokenType::IDENTIFIER, match.str(), current_line, current_col, path)); }

        current_col++;
    }

    return clean(tokens);
}
