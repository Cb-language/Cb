#include "CtorWithReturnValue.h"

CtorWithReturnValue::CtorWithReturnValue(const Token& token) : Error(token, "A constructor cant have a return type")
{
}
