#pragma once
#include "errorHandling/Error.h"

class NoLineOpener : public Error
{
public:
    explicit NoLineOpener(const Token& token);
};
