#include "Tokenizer.h"

#include "TrieTree/Keywords.h"

boost::regex Tokenizer::tokenRegex;

const std::vector<std::string> Tokenizer::captureBlocks = {
    R"((?<ConstFloat>\d*\.\d+))",
    R"((?<ConstInt>\d+))",
    R"((?<ConstChar>'(\\.|[^\\'\n])'))",
    R"((?<ConstStr>"(\\.|[\s\S])*?"))",
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

std::vector<Token> Tokenizer::tokenizeByTrieTree(const std::string& code, const std::filesystem::path& path) const
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

        // If not a keyword, resort word a REGEX const/identifier check.
        const auto match = *boost::regex_iterator(code.c_str() + i, code.c_str() + code.length(), LEXER_RGX);
        bool matched = false;

        for (int j = 1; j <= TOKEN_GROUP_RGX_COUNT; ++j)
        {
            if (const auto &match_info = match[j]; match_info.matched)
            {
                std::unique_ptr<token> token = token::from_regex_group_id(current_code_position, j, match_info);

                on_regex_token(token.get());
                tokens.push(std::move(token));

                i += match_info.length();
                matched = true;
                break;
            }
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

size_t Tokenizer::handleKeywordMatch(const std::string& code, size_t& row, size_t& col, const TokenType tokenType,
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

        case TokenType::PUNCTUATION_NEW_LINE:
        {
            row += 1;
            col = 0;
        }

        default:
        {
            tokens.emplace_back(tokenType, std::nullopt, row, col, path);
            return keywordEnd;
        }
    }
}

void Tokenizer::on_regex_token(Token* token)
{
    switch (token->type)
    {
        case TokenType::CONST_STR:
        {
            auto *iden_token = static_cast<codesh::identifier_token *>(token); // NOLINT(*-pro-type-static-cast-downcast)
            std::string content = iden_token->get_content();

            // Handle newline
            // We want to replace "newline" but not "no newline".
            // To not create a conflict and unnecessary spaghetti code, will simply resort to REGEX:
            content = boost::regex_replace(content, NEWLINE_REPLACE_RGX, " \n ");

            // Remove string enclose
            content = content
                .substr(
                    trie::keyword::STRING_OPEN.length(),
                    content.length() - trie::keyword::STRING_OPEN.length() - trie::keyword::STRING_END.length()
                );

            // Replace escaped characters
            escape_characters(content, trie::keyword::STRING_END.substr(1));
            escape_characters(content, trie::keyword::STRING_NEWLINE.substr(1));

            iden_token->set_content(content);
        }

        default:
            break;
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
