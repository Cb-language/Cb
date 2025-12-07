#include "InvalidMainReturnType.h"

InvalidMainReturnType::InvalidMainReturnType(const Token& token) : Error(token, "prelude return type must be fermata")
{
}
