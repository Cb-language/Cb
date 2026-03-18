#pragma once
#include "errorHandling/Error.h"

class PureWithBody : public Error
{
public:
    explicit PureWithBody(const Token& token);
};
