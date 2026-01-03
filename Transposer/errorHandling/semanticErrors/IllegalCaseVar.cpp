#include "IllegalCaseVar.h"

IllegalCaseVar::IllegalCaseVar(const Token& token, const std::string& name) : Error(token, "Case must have the variable (" + name + ") to be degreeable")
{
}
