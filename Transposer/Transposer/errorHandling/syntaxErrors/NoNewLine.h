#pragma once
#include "errorHandling/Error.h"

class NoNewLine : public Error
{
public:
    explicit NoNewLine(const Token& token);
};
