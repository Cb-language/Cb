#pragma once
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"
#include <sstream>

class WhileStmt : public Stmt
{
private:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
public:
    WhileStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Expr>& condition, std::unique_ptr<Stmt>& body);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};