#pragma once
#include "errorHandling/Error.h"

class StmtNotContinueAble : public Error
{
public:
    explicit StmtNotContinueAble(const Token& token);
};
