#include "MissingIdentifier.h"

MissingIdentifier::MissingIdentifier(const Token &token) : Error(token, "missing identifier")
{
}