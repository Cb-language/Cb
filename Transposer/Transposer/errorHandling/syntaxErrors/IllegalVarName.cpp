#include "IllegalVarName.h"

IllegalVarName::IllegalVarName(const Token& token) : Error(token, "var's name must not start with a note")
{
}
