#include "ClassDosentExisit.h"

ClassDosentExisit::ClassDosentExisit(const Token& token, const std::string& className) : Error(token, "Type: " + className + " doesn't exists")
{
}
