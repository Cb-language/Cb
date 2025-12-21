#pragma once
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class CaseStmt : public Stmt
{
private:
    std::unique_ptr<Stmt> body;
    std::unique_ptr<Expr> expr;
    bool isDefault = false;
public:
    CaseStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Stmt> body, std::unique_ptr<Expr> expr, const bool isDefault = false);
    bool isLegal() const override;
    std::string translateToCpp() const override;

};
