#pragma once
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class WhileStmt : public Stmt
{
private:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
public:
    WhileStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, std::unique_ptr<Expr>& condition, std::unique_ptr<Stmt>& body);
    void analyze() const override;
    std::string translateToCpp() const override;
};