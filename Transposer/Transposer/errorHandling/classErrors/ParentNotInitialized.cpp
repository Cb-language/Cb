#include "ParentNotInitialized.h"

ParentNotInitialized::ParentNotInitialized(const Token& token, const std::string& className, const std::string& parentName) 
    : Error(token, "Parent class " + parentName + " must be initialized in " + className + " constructor")
{
}
