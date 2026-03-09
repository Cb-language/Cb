#pragma once
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class WhileStmt : public Stmt
{
private:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
public:
    WhileStmt(const Token& token, IFuncDeclStmt* funcDecl, std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
};