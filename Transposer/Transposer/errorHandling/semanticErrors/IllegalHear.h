#pragma once
#include "errorHandling/Error.h"

class IllegalHear : public Error
{
public:
    IllegalHear(const Token& token, const std::string& type);
};
