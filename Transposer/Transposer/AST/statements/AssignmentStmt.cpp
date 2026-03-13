#include "AssignmentStmt.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"

AssignmentStmt::AssignmentStmt(const Token& token,
                               std::unique_ptr<Call> call, const std::string& assignmentOp, std::unique_ptr<Expr> expr)
        : Expr(token), call(std::move(call)), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

void AssignmentStmt::analyze() const
{
    if (symTable == nullptr) return;

    call->setSymbolTable(symTable);
    call->setScope(scope);
    call->setClassNode(currClass);
    call->analyze();

    expr->setSymbolTable(symTable);
    expr->setScope(scope);
    expr->setClassNode(currClass);
    expr->analyze();

    const auto leftType = call->getType();

    if (const auto rightType = expr->getType(); leftType->toString() != rightType->toString())
    {
        throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), assignmentOp);
    }
}

std::string AssignmentStmt::translateToCpp() const
{
    std::string res = needsSemicolon ? getTabs() : "";
    res += call->translateToCpp() + " " + assignmentOp + " " + expr->translateToCpp();
    if (needsSemicolon) res += ";";
    return res;
}

std::unique_ptr<IType> AssignmentStmt::getType() const
{
    return call->getType()->copy();
}
