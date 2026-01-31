#include "NoReturn.h"

NoReturn::NoReturn(const Token& token) : Error(token, "no return statement")
{
}
