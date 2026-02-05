#pragma once
#include "errorHandling/Error.h"

class MissingClassPipe : public Error
{
public:
    explicit MissingClassPipe(const Token& token);
};
