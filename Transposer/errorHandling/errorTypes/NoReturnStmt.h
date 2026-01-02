#pragma once
#include "../Error.h"

class NoReturnStmt : public Error
{
public:
    NoReturnStmt(const Token& token);
};
