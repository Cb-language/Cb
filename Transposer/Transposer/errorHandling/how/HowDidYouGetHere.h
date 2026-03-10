#pragma once
#include "errorHandling/Error.h"

class HowDidYouGetHere : public Error
{
public:
    explicit HowDidYouGetHere(const Token &token);
};
