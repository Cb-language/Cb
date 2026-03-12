#pragma once
#include "errorHandling/Error.h"

class InvalidCtorName : public Error
{
public:
    InvalidCtorName(const Token& token, const std::string& className);
};
