#pragma once
#include "../Error.h"

class NoReturnStmt : Error
{
public:
    NoReturnStmt(const Token& token);
};
