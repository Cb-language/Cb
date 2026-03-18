#include "Error.h"

#include "other/Utils.h"
#include "token/TrieTree/Keywords.h"

std::unordered_map<CbTokenType, std::string> Error::tokenToTypeMap = typeToStr();

std::unordered_map<CbTokenType, std::string> Error::typeToStr()
{
    std::unordered_map<CbTokenType, std::string> result;

    for (const auto &entry : KEYWORDS)
    {
        result.emplace(entry.type, entry.keyword);
    }

    return result;
}

Error::Error(const Token &token, const std::string& errorMessage) : token(token), errorMessage(errorMessage)
{
    fullMessage = this->errorMessage +
                   " in file: \"" + token.path.string() + "\"" +
                   " at line: " + std::to_string(token.line) +
                   " at column: " + std::to_string(token.column);

    if (tokenToTypeMap.contains(token.type))
    {
        fullMessage += " near token: \"" + tokenToTypeMap.at(token.type) + "\"\n";
    }

}

void Error::print() const
{
    std::cout << fullMessage << std::endl;
}

const char* Error::what() const noexcept
{
    return fullMessage.c_str();
}

const Token& Error::getToken() const
{
    return token;
}

const std::string& Error::getErrorMessage() const
{
    return errorMessage;
}

Error* Error::copy() const
{
    return new Error(token, errorMessage);
}
