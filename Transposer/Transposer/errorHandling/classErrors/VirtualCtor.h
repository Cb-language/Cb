#pragma once
#include "errorHandling/Error.h"

class VirtualCtor : public Error
{
public:
    explicit VirtualCtor(const Token& token);
};
