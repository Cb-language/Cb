#pragma once
#include "errorHandling/Error.h"

class MissingPipe : public Error
{
public:
    explicit MissingPipe(const Token& token);
};
