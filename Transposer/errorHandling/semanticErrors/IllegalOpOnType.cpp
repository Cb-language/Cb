#include "IllegalOpOnType.h"

IllegalOpOnType::IllegalOpOnType(const Token& token, const std::string& op, const std::string& type) : Error(token, "Can't do " + op + " on " + type)
{
}
