#pragma once

#include "Statement.h"
#include "other/Type.h"

class Expr : public Stmt
{
private:
    int line;
    int column;
public:
    Expr(const int line, const int column) : line(line), column(column) {}
    ~Expr() override = default;
    virtual Type getType() const = 0;
    bool isLegal() const override = 0;
    std::string translateToCpp() const override = 0;
};