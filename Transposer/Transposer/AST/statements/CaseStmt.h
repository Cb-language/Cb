#pragma once
#include "BodyStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class CaseStmt : public Stmt
{
private:
    std::unique_ptr<Expr> expr;
    std::unique_ptr<BodyStmt> body;
    bool isDefault = false;
public:
    CaseStmt(const Token& token, IFuncDeclStmt* funcDecl,
        std::unique_ptr<Expr> expr, std::unique_ptr<BodyStmt> body, const bool isDefault = false, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;

};
