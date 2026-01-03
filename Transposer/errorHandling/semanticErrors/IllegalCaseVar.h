#pragma once
#include "errorHandling/Error.h"

class IllegalCaseVar : public Error
{
public:
    IllegalCaseVar(const Token& token, const std::string& name);
};
