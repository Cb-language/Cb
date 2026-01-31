#include "IllegalTypeCast.h"

IllegalTypeCast::IllegalTypeCast(const Token& token, const std::string& typeFrom, const std::string& typeTo)
    : Error(token, "Can't cast " + typeFrom + " to " + typeTo)
{
}