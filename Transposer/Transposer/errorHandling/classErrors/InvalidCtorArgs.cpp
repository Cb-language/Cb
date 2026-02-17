#include "InvalidCtorArgs.h"

InvalidCtorArgs::InvalidCtorArgs(const Token& token) : Error(token, "No constructor found with those arguments")
{
}
