#pragma once
#include "errorHandling/Error.h"

class IllegalCall : public Error
{
public:
    IllegalCall(const Token& token, const std::string& name);
};
