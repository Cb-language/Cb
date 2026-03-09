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
    const std::string varName;

public:
    ForStmt(const Token& token, IFuncDeclStmt* funcDecl,
        std::unique_ptr<BodyStmt> body, const bool isIncreasing, std::unique_ptr<Expr> startExpr,
        std::unique_ptr<Expr> stepExpr, std::unique_ptr<Expr> stopExpr, const std::string& varName, ClassDeclStmt* classDecl = nullptr);

    void analyze() const override;
    std::string translateToCpp() const override;
};
