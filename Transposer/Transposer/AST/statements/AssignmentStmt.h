#pragma once
#include <memory>

#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"
#include "expression/VarCallExpr.h"
#include "../../symbols/Var.h"

class AssignmentStmt : public Expr
{
private:
    const std::unique_ptr<Call> call;
    const std::string assignmentOp;
    const std::unique_ptr<Expr> expr;

public:
    AssignmentStmt(const Token& token,
        std::unique_ptr<Call> call, const std::string& assignmentOp, std::unique_ptr<Expr> expr);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
};
