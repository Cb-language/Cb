#include "UnrecognizedToken.h"

UnrecognizedToken::UnrecognizedToken(const Token& token) : Error(token, "This Token is unrecognized")
{
}
