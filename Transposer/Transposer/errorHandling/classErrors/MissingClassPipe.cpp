#include "MissingClassPipe.h"

MissingClassPipe::MissingClassPipe(const Token& token) : Error(token, "Missing | between fields and methods")
{
}
