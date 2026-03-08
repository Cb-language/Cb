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
        boost::smatch match;
        std::string search_target = code.substr(codePos);
        if (boost::regex_search(search_target, match, tokenRegex, boost::regex_constants::match_continuous))
        {
            auto type = TokenType::ERROR_TOKEN;
            if (match["ConstFloat"].matched) type = TokenType::CONST_FLOAT;
            else if (match["ConstInt"].matched) type = TokenType::CONST_INT;
            else if (match["ConstChar"].matched) type = TokenType::CONST_CHAR_OPEN;
            else if (match["ConstStr"].matched) type = TokenType::CONST_STR_OPEN;
            else if (match["Identifier"].matched) type = TokenType::IDENTIFIER;

            if (type != TokenType::ERROR_TOKEN)
            {
                std::string match_str = match.str();
                Token token(type, match_str, row, col, path);
                onRegexToken(&token);
                tokens.emplace_back(token);

                codePos += match_str.length();
                col += match_str.length() - 1; // col will be incremented at the start of next loop
                continue;
            }
        }

        // If nothing matched, skip one character or handle as error
        codePos++;
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

void Tokenizer::onRegexToken(Token* token)
{
    if (!token->value.has_value()) return;

    switch (token->type)
    {
        case TokenType::CONST_STR_OPEN:
        {
            std::string content = token->value.value();

            // Handle newline
            // We want to replace "newline" but not "no newline".
            // To not create a conflict and unnecessary spaghetti code, will simply resort to REGEX:
            static const boost::regex NEWLINE_REPLACE_RGX(R"((?<!no )newline)");
            content = boost::regex_replace(content, NEWLINE_REPLACE_RGX, " \n ");

            // Remove string enclose
            if (content.length() >= 2 && content.front() == '\"' && content.back() == '\"')
            {
                content = content.substr(1, content.length() - 2);
            }

            // Replace escaped characters
            static const boost::regex escapedQuote(R"(\\")");
            content = boost::regex_replace(content, escapedQuote, "\"");

            static const boost::regex escapedNewline(R"(\\n)");
            content = boost::regex_replace(content, escapedNewline, "\n");

            static const boost::regex escapedBackslash(R"(\\\\)");
            content = boost::regex_replace(content, escapedBackslash, "\\");

            token->value = content;
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