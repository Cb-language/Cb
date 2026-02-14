#pragma once
#include "errorHandling/Error.h"

class AccessError : public Error
{
public:
    AccessError(const Token& token, const std::string& className, const std::string& itemName);
};
