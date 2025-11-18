#pragma once

#include "Statement.h"
#include "other/Type.h"

class Expr : public Stmt
{
public:
    Expr() = default;
    ~Expr() override = default;
    virtual Type getType() const = 0;
    bool isLegal() const override = 0;
    std::string translateToCpp() const override = 0;
};