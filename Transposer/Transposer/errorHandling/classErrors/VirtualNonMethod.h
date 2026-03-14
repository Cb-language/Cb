#pragma once
#include "errorHandling/Error.h"

class VirtualNonMethod : public Error
{
public:
    explicit VirtualNonMethod(const Token& token);
};
