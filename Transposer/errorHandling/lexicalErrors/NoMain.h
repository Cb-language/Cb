#pragma once
#include "errorHandling/Error.h"

class NoMain : Error
{
public:
    NoMain(const Token &token);
};
