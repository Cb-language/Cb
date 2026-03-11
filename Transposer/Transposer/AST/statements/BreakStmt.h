#pragma once
#include "AST/abstract/Statement.h"

class BreakStmt : public Stmt
{
private:

public:
    explicit BreakStmt(const Token& token);
    void analyze() const override;
    std::string translateToCpp() const override;
};
