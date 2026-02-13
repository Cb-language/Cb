#include "AccessError.h"

AccessError::AccessError(const Token& token, const std::string& className, const std::string& itemName) : Error(token, itemName + " isn't accessible in " + className)
{
}
