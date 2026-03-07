#include "IdentifierTaken.h"

#include "other/Utils.h"

IdentifierTaken::IdentifierTaken(const Token& token) : Error(token, "identifier already taken")
{
    fullMessage = (this->errorMessage +
                   " at line: " + std::to_string(token.line) +
                   " at column: " + std::to_string(token.column) +
                   " identifier: " + token.value) + "\n";
}
