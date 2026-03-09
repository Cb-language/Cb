#pragma once
#include "errorHandling/Error.h"

class HowDareYou : public Error
{
public:
    HowDareYou(const Token& token);
};
