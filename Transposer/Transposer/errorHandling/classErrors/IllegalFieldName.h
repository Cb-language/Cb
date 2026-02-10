#pragma once
#include "errorHandling/Error.h"

class IllegalFieldName : public Error
{
public:
    explicit IllegalFieldName(const Token& token);
};
