#include "IllegalHear.h"

IllegalHear::IllegalHear(const Token& token, const std::string& type) : Error(token, "Can't hear into " + type)
{
}
