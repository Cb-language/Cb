#pragma once
#include "errorHandling/Error.h"

class NoLineOpener : public Error
{
public:
    NoLineOpener(const Token& token);
};
