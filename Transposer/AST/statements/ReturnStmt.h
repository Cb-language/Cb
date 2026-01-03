#pragma once

#include "FuncDeclStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class ReturnStmt : public Stmt
{
private:
    std::unique_ptr<IType> rType;
    std::unique_ptr<Expr> rExpr;

public:
    ReturnStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Expr>& rExpr);

    void analyze() const override;
    std::string translateToCpp() const override;

};
