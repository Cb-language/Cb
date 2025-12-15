#pragma once
#include "BodyStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class ElseIfStmt : public Stmt
{
private:
    std::unique_ptr<Expr> expr;
    std::unique_ptr<BodyStmt> body;
public:
    ElseIfStmt(Scope* scope, FuncDeclStmt* func_decl, std::unique_ptr<Expr>& expr, std::unique_ptr<BodyStmt>& body);

    bool isLegal() const override;
    std::string translateToCpp() const override;

};
