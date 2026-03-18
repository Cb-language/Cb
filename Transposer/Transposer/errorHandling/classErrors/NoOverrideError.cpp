#include "NoOverrideError.h"

NoOverrideError::NoOverrideError(const Token& token)
: Error(token, "Method marked is virtual in base class but isn't overridden in derived class")
{
}