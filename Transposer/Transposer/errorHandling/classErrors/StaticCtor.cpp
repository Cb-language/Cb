#include "StaticCtor.h"

StaticCtor::StaticCtor(const Token& token) : Error(token, "Constructor can't be static")
{
}
