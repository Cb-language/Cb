#include "Error.h"
#include <iostream>
#include <utility>

Error::Error(const int line, const int column, std::string errorMessage, std::string token) : line(line), column(column), errorMessage(std::move(errorMessage)), token(std::move(token))
{
    print();
}

void Error::print() const
{
    std::cout << errorMessage;
    if (!token.empty())
        std::cout << " near token '" << token << "'";
    std::cout << " at line: " << line
              << " at column: " << column
              << std::endl;
}

