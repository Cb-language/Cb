#pragma once
#include "errorHandling/Error.h"

class HowDidYouGetHere : public Error
{
public:
    HowDidYouGetHere(const Token &token);
};
