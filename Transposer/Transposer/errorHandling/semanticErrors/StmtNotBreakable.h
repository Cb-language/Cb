#pragma once
#include "errorHandling/Error.h"

class StmtNotBreakable : public Error
{
public:
    explicit StmtNotBreakable(const Token& token);
};
