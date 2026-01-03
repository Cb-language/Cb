#include "IllegalOpOnType.h"

IllegalOpOnType::IllegalOpOnType(const Token& token, const std::string& type) : Error(token, "Can't do that op on " + type)
{
}
