#include "InvalidMainArgs.h"

InvalidMainArgs::InvalidMainArgs(const Token& token) : Error(token, "prelude must have no arguments")
{
}
