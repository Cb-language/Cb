#pragma once
#include <memory>

#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"
#include "expression/VarCallExpr.h"
#include "../../symbols/Var.h"

class AssignmentStmt : public Stmt
{
private:
    const std::unique_ptr<Call> call;
    const std::string assignmentOp;
    const std::unique_ptr<Expr> expr;

public:
    AssignmentStmt(const Token& token, IFuncDeclStmt* funcDecl,
        std::unique_ptr<Call> call, const std::string& assignmentOp, std::unique_ptr<Expr> expr, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
};
