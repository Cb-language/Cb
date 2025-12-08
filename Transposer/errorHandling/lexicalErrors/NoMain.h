#pragma once
#include "errorHandling/Error.h"

class NoMain : public Error
{
public:
    NoMain(const Token &token);
};
