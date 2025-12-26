#pragma once
#include "BodyStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class ForStmt: public Stmt
{
private:
    const std::unique_ptr<BodyStmt> body;
    const bool isIncreasing;
    const std::unique_ptr<Expr> startExpr;
    const std::unique_ptr<Expr> stepExpr;
    const std::unique_ptr<Expr> stopExpr;
    const std::wstring varName;

public:
    ForStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<BodyStmt> body, const bool isIncreasing, std::unique_ptr<Expr> startExpr,
        std::unique_ptr<Expr> stepExpr, std::unique_ptr<Expr> stopExpr, const std::wstring& varName);

    bool isLegal() const override;
    std::string translateToCpp() const override;
};
