#include "IllegalOpOnType.h"

IllegalOpOnType::IllegalOpOnType(const Token& token, const std::string& type) : Error(token, "Can't do that op on " + type)
{
}

IllegalOpOnType::IllegalOpOnType(const Token& token, const std::string& type1, const std::string& type2,
    const std::string& op) : Error(token, "Can't do \"" + op + "\" on types: " + type1 + ", " + type2)
{
}
