#pragma once
#include "errorHandling/Error.h"

class NoMain : public Error
{
public:
    explicit NoMain(const Token &token);
};
