#pragma once
#include "errorHandling/Error.h"

class StaticCtor : public Error
{
public:
    explicit StaticCtor(const Token& token);
};
