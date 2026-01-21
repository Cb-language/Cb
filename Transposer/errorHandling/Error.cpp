#include "Error.h"

#include "other/Utils.h"

Error::Error(const Token &token, const std::string& errorMessage) : token(token), errorMessage(std::move(errorMessage))
{
    fullMessage = (this->errorMessage +
                   " in file: \"" + token.path.string() + "\"" +
                   " at line: " + std::to_string(token.line) +
                   " at column: " + std::to_string(token.column) +
                   " near token: " + Utils::wstrToStr(token.value) + "\n");

}

void Error::print() const
{
    std::cout << fullMessage << std::endl;
}

const char* Error::what() const noexcept
{
    return fullMessage.c_str();
}