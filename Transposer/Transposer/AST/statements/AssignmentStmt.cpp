#include "AssignmentStmt.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"

AssignmentStmt::AssignmentStmt(const Token& token,
                               std::unique_ptr<Call> call, const std::string& assignmentOp, std::unique_ptr<Expr> expr)
        : Expr(token), call(std::move(call)), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

void AssignmentStmt::analyze() const
{
    const auto leftType = call->getType();

    if (const auto rightType = expr->getType(); *leftType != *rightType)
    {
        throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), assignmentOp);
    }
}

std::string AssignmentStmt::translateToCpp() const
{
    return getTabs() + call->translateToCpp() + " " + assignmentOp + " " + expr->translateToCpp() + ";";
}

std::unique_ptr<IType> AssignmentStmt::getType() const
{
    return call->getType()->copy();
}
