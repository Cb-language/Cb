#pragma once
#include "BodyStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class IfStmt : public Stmt
{
private:
    const Expr *expr;
    BodyStmt* body;
public:
    IfStmt(Scope* scope, FuncDeclStmt* funcDecl, const Expr* expr, BodyStmt* body);

    bool isLegal() const override;
    std::string translateToCpp() const override;

    const Expr *getExpression() const;
};
