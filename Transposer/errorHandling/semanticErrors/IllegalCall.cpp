#include "IllegalCall.h"

IllegalCall::IllegalCall(const Token& token, const std::string& name) : Error(token, "Illegal call on function with name: " + name)
{
}
