#pragma once
#include "errorHandling/Error.h"

class HowDareYou : public Error
{
public:
    explicit HowDareYou(const Token& token);
};
