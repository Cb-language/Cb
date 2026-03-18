#include "OverrideError.h"

OverrideError::OverrideError(const Token& token) : Error(token, "Method marked as override does not override a virtual method from a base class")
{
}