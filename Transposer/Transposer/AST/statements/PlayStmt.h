#pragma once

#include "AST/abstract/Statement.h"
#include "expression/ConstValueExpr.h"

class PlayStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Expr>> exprs;
    bool printLine = false;
public:
    PlayStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, std::vector<std::unique_ptr<Expr>> exprs, bool printLine = false);
    void analyze() const override;
    std::string translateToCpp() const override;
};