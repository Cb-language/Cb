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
    const std::wstring assignmentOp;
    const std::unique_ptr<Expr> expr;

public:
    AssignmentStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, std::unique_ptr<Call> call, const std::wstring& assignmentOp, std::unique_ptr<Expr> expr);
    void analyze() const override;
    std::string translateToCpp() const override;
};
