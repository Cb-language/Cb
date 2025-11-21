#pragma once

#include "AST/abstract/Statement.h"
#include "expression/ConstValueExpr.h"

class PlayStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Expr>> vars;
    bool printLine = false;
public:
    PlayStmt(std::vector<std::unique_ptr<Expr>>& vars, bool printLine = false);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};