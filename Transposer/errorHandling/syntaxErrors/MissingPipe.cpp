#include "MissingPipe.h"

MissingPipe::MissingPipe(const Token& token) : Error(token, "Missing | to open case")
{
}