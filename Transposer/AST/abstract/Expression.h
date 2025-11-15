#pragma once

#include "Statement.h"
#include "AST/other/Type.h"

class Expression : public Stmt
{
private:
    int line;
    int column;
public:
    Expression(const int line, const int column) : line(line), column(column) {}
    ~Expression() override = default;
    virtual Type getType() const = 0;
    bool isLegal() const override = 0;
};