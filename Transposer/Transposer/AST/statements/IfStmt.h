#pragma once
#include "BodyStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"
#include <sstream>

class IfStmt : public Stmt
{
private:
    std::unique_ptr<Expr> expr;
    std::unique_ptr<Stmt> body;
    std::unique_ptr<Stmt> elseIfStmt;

    const bool isElseIf = false;
public:
    IfStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body, std::unique_ptr<Stmt> elseIfStmt, const bool isElseIf);

    void analyze() const override;
    std::string translateToCpp() const override;
};