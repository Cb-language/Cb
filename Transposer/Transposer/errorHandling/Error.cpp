#include "Error.h"

#include "other/Utils.h"

Error::Error(const Token &token, const std::string& errorMessage) : token(token), errorMessage(errorMessage)
{
    fullMessage = (this->errorMessage +
                   " in file: \"" + token.path.string() + "\"" +
                   " at line: " + std::to_string(token.line) +
                   " at column: " + std::to_string(token.column) +
                   " near token: \"" + token.value + "\"\n");

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
