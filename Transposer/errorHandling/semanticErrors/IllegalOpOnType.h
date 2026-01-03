#pragma once
#include "errorHandling/Error.h"

class IllegalOpOnType : public Error
{
public:
    IllegalOpOnType(const Token& token, const std::string& op, const std::string& type);
};
