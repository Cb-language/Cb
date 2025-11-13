#include "Error.h"
#include <iostream>
#include <utility>

Error::Error(const int line, const int column, std::string errorMessage, std::string token) : line(line), column(column), errorMessage(std::move(errorMessage)), token(std::move(token))
{
    fullMessage = (this->errorMessage +
                   " at line: " + std::to_string(line) +
                   " at column: " + std::to_string(column) +
                   " near token: " + this->token);

}

void Error::print() const
{
    std::cout << fullMessage << std::endl;
}

const char* Error::what() const noexcept
{
    return fullMessage.c_str();
}