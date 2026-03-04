#pragma once
#include "errorHandling/Error.h"

class VirtualNonMethod : public Error
{
public:
    VirtualNonMethod(const Token& token);
};
