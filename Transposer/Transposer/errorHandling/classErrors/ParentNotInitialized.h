#pragma once
#include "errorHandling/Error.h"

class ParentNotInitialized : public Error
{
public:
    ParentNotInitialized(const Token& token, const std::string& className, const std::string& parentName);
};
