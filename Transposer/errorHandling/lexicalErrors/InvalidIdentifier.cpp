#include "InvalidIdentifier.h"

#include "errorHandling/Error.h"

InvalidIdentifier::InvalidIdentifier(const Token& token) : Error(token, "invalid identifier")
{
}
