#include "AssignmentStmt.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"

AssignmentStmt::AssignmentStmt(const Token& token, IFuncDeclStmt* funcDecl,
                               std::unique_ptr<Call> call, const std::string& assignmentOp, std::unique_ptr<Expr> expr, ClassDeclStmt* classDecl)
        : Expr(token, funcDecl, classDecl), call(std::move(call)), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

void AssignmentStmt::analyze() const
{
    auto leftType = call->getType();
    auto rightType = expr->getType();

    if (*leftType != *rightType)
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
