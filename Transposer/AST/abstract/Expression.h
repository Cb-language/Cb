#pragma once

#include "Statement.h"
#include "AST/other/Type.h"

class Expression : public Stmt
{
public:
    virtual Type getType() const = 0;
};