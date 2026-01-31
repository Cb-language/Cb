#pragma once
#include "errorHandling/Error.h"

class MissingPipe : public Error
{
public:
    MissingPipe(const Token& token);
};
