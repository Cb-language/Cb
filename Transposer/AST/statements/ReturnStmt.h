#pragma once

#include "FuncDeclStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class ReturnStmt : public Stmt
{
private:
    const Type rType;
    const Expr* rExpr;

public:
    ReturnStmt(Scope* scope, FuncDeclStmt* funcDecl, const Type& rType, const Expr* rExpr);

    bool isLegal() const override;
    std::string translateToCpp() const override;

};
