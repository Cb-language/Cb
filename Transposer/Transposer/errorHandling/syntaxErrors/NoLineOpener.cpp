#include "NoLineOpener.h"

NoLineOpener::NoLineOpener(const Token& token) : Error(token, "expected a line opening")
{
}
