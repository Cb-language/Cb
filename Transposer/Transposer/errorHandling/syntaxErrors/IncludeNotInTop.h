#pragma once
#include "errorHandling/Error.h"

class IncludeNotInTop : public Error
{
public:
    IncludeNotInTop(const Token& token);
};
