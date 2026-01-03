#pragma once
#include "errorHandling/Error.h"

class StmtNotContinueAble : public Error
{
public:
    StmtNotContinueAble(const Token& token);
};
