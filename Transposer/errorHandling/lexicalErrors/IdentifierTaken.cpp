#include "IdentifierTaken.h"

IdentifierTaken::IdentifierTaken(const Token& token) : Error(token, "identifier taken")
{
}
