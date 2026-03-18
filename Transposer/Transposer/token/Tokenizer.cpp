#include "Tokenizer.h"

#include "TrieTree/Keywords.h"

boost::regex Tokenizer::tokenRegex;

const std::vector<std::string> Tokenizer::captureBlocks = {
    R"((?<ConstFloat>\d*\.\d+))",
    R"((?<ConstInt>\d+))",
    R"((?<ConstChar>'(\\.|[^\\'\n])'))",
    R"((?<ConstStr>"(\\.|[\s\S])*?"))",
    R"((?<ConstBool>\b(?:cres|demen)\b))",
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

static void updatePos(const std::string& text, size_t& row, size_t& col)
{
    for (char c : text)
    {
        if (c == '\n')
        {
            row++;
            col = 1;
        }
        else
        {
            col++;
        }
    }
}

size_t Tokenizer::handleKeywordMatch(const std::string& code, size_t& row, size_t& col, const CbTokenType tokenType,
            std::queue<Token>& tokens, const size_t keywordEnd, const size_t keywordStart, const std::filesystem::path& path)
{
    const std::string matchedText = code.substr(keywordStart, keywordEnd - keywordStart);
    
    switch (tokenType)
    {
        case CbTokenType::COMMENT_SINGLE:
        {
            // If it's a one-line comment, look for the end of the line.
            size_t end = code.find('\n', keywordEnd);
            if (end == std::string::npos) end = code.size();
            
            updatePos(code.substr(keywordStart, end - keywordStart), row, col);
            return (end < code.size()) ? end : code.size();
        }

        case CbTokenType::COMMENT_MULTI_START:
        {
            // If it's a multiline comment, look for a COMMENT_MULTY_END match.
            size_t end = code.find(COMMENT_MULTY_END, keywordEnd);
            if (end != std::string::npos)
            {
                size_t actualEnd = end + COMMENT_MULTY_END.length();
                updatePos(code.substr(keywordStart, actualEnd - keywordStart), row, col);
                return actualEnd;
            }

            tokens.emplace(CbTokenType::ERROR_TOKEN, std::nullopt, row, col, path);
            updatePos(matchedText, row, col);
            return keywordEnd;
        }

        default:
        {
            tokens.emplace(tokenType, std::nullopt, row, col, path);
            updatePos(matchedText, row, col);
            return keywordEnd;
        }
    }
}

void Tokenizer::onRegexToken(Token* token)
{
    if (!token->value.has_value()) return;
    // for post analysis if needed
    switch (token->type)
    {
        case CbTokenType::CONST_STR:
        {
            std::string content = token->value.value();
            if (content.length() >= 2 && content.front() == '\"' && content.back() == '\"')
            {
                content = content.substr(1, content.length() - 2);
            }
            token->value = content;
            break;
        }

        case CbTokenType::CONST_CHAR:
        {
            std::string content = token->value.value();
            if (content.length() >= 2 && content.front() == '\'' && content.back() == '\'')
            {
                content = content.substr(1, content.length() - 2);
            }
            token->value = content;
            break;
        }

    default:
        break;
    }
}

Tokenizer::Tokenizer() : trieTree(std::make_unique<TrieNode>())
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

std::queue<Token> Tokenizer::tokenize(const std::string& code, const std::filesystem::path& path) const
{
    std::queue<Token> tokens;
    size_t row = 1;
    size_t col = 1;
    size_t codePos = 0;

    while (codePos < code.size())
    {
        if (isspace(code[codePos]) && code[codePos] != '\n')
        {
            col++;
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
        }

        if (lastMatch && checkBoundary(code, lastMatch, codePos, lastMatchEnd))
        {
            codePos = handleKeywordMatch(code, row, col, lastMatch->type, tokens, lastMatchEnd, codePos, path);
            continue;
        }

        // If not a keyword, resort word a REGEX const/identifier check.
        std::string search_target = code.substr(codePos);
        if (boost::smatch match; boost::regex_search(search_target, match, tokenRegex, boost::regex_constants::match_continuous))
        {
            auto type = CbTokenType::ERROR_TOKEN;
            if (match["ConstFloat"].matched) type = CbTokenType::CONST_FLOAT;
            else if (match["ConstInt"].matched) type = CbTokenType::CONST_INT;
            else if (match["ConstChar"].matched) type = CbTokenType::CONST_CHAR;
            else if (match["ConstStr"].matched) type = CbTokenType::CONST_STR;
            else if (match["ConstBool"].matched) type = CbTokenType::CONST_BOOL;
            else if (match["Identifier"].matched) type = CbTokenType::IDENTIFIER;

            std::string match_str = match.str();
            Token token(type, match_str, row, col, path);
            onRegexToken(&token);
            tokens.emplace(token);

            updatePos(match_str, row, col);
            codePos += match_str.length();
            continue;
        }

        tokens.emplace(CbTokenType::ERROR_TOKEN, std::nullopt, row, col, path); // if didnt match throw error
        col++;
        codePos++;
    }
    return tokens;
}
