#pragma once
#include "errorHandling/Error.h"

class IncludeNotInTop : public Error
{
public:
    explicit IncludeNotInTop(const Token& token);
};
