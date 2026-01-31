#pragma once
#include "errorHandling/Error.h"

class IllegalCredit : public Error
{
public:
    IllegalCredit(const Token& token, const std::string& name);
};
