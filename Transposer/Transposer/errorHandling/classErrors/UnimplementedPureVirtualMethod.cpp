#include "UnimplementedPureVirtualMethod.h"

UnimplementedPureVirtualMethod::UnimplementedPureVirtualMethod(const Token& token, const std::string& methodName) : Error(token, "Class does not implement pure virtual method '" + methodName + "' from base class")
{
}