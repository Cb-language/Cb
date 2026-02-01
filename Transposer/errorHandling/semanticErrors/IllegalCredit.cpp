#include "IllegalCredit.h"

IllegalCredit::IllegalCredit(const Token& token, const std::string& name) : Error(token, "Illegal credit on function with name: " + name)
{
}
