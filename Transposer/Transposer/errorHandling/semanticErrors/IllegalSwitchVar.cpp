#include "IllegalSwitchVar.h"

IllegalSwitchVar::IllegalSwitchVar(const Token& token, const std::string& name) : Error(token, "Case must have the variable (" + name + ") to be degreeable")
{
}
