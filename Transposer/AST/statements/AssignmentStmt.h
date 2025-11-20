#pragma once
#include <memory>

#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"
#include "other/Var.h"

class AssignmentStmt : public Stmt
{
private:
    const Var var;
    const std::wstring assignmentOp;
    const std::unique_ptr<Expr> expr;

public:
    AssignmentStmt(const Var& var, const std::wstring& assignmentOp, std::unique_ptr<Expr> expr);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
