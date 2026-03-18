#pragma once
#include "errorHandling/Error.h"

class CaseNotInsideIf : public Error
{
public:
    CaseNotInsideIf(const Token& token);
};
