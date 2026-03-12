#include "InvalidCtorName.h"

InvalidCtorName::InvalidCtorName(const Token& token, const std::string& className) : Error(token, "this constructor has the wrong name for: " + className)
{
}
