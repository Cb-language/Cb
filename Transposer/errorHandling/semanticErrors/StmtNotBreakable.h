#pragma once
#include "errorHandling/Error.h"

class StmtNotBreakable : public Error
{
public:
    StmtNotBreakable(const Token& token);
};
