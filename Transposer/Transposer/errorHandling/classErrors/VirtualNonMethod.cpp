#include "VirtualNonMethod.h"

VirtualNonMethod::VirtualNonMethod(const Token& token) : Error(token, "Only methods can be virtual, override or pure virtual")
{
}
