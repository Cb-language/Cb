#pragma once
#include "errorHandling/Error.h"

class NoNewLine : public Error
{
public:
    NoNewLine(const Token& token);
};
