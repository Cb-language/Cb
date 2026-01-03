#pragma once

#include "AST/abstract/Statement.h"
#include "expression/ConstValueExpr.h"

class PlayStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Expr>> exprs;
    bool printLine = false;
public:
    PlayStmt(Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Expr>> exprs, bool printLine = false);
    void analyze() const override;
    std::string translateToCpp() const override;
};