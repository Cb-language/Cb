#pragma once
#include "errorHandling/Error.h"

class IllegalLengthCall : public Error
{
public:
    IllegalLengthCall(const Token& token, const std::string& type);
};
