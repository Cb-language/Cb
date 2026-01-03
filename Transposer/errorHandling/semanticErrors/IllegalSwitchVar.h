#pragma once
#include "errorHandling/Error.h"

class IllegalSwitchVar : public Error
{
public:
    IllegalSwitchVar(const Token& token, const std::string& name);
};
