#include "Error.h"

Error::Error(const Token &token, std::string errorMessage) : token(token), errorMessage(std::move(errorMessage))
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::string token_name = conv.to_bytes(token.value);

    fullMessage = (this->errorMessage +
                   " at line: " + std::to_string(token.line) +
                   " at column: " + std::to_string(token.column) +
                   " near token: " + token_name + "\n");

}

void Error::print() const
{
    std::cout << fullMessage << std::endl;
}

const char* Error::what() const noexcept
{
    return fullMessage.c_str();
}