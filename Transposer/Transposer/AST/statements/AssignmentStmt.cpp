#include "AssignmentStmt.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"

AssignmentStmt::AssignmentStmt(const Token& token,
                               std::unique_ptr<VarReference> ref, const std::string& assignmentOp, std::unique_ptr<Expr> expr)
        : Expr(token), varRef(std::move(ref)), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

void AssignmentStmt::analyze() const
{
    if (symTable == nullptr) return;

    varRef->setSymbolTable(symTable);
    varRef->setScope(scope);
    varRef->setClassNode(currClass);
    varRef->analyze();

    expr->setSymbolTable(symTable);
    expr->setScope(scope);
    expr->setClassNode(currClass);
    expr->analyze();

    const auto leftType = varRef->getType();
    if (const auto rightType = expr->getType(); rightType == nullptr || *leftType != *rightType)
    {
        if (rightType != nullptr) symTable->addError(std::make_unique<IllegalOpOnType>(token, leftType->toString(), rightType->toString(), assignmentOp));
    }
}

std::string AssignmentStmt::translateToCpp() const
{
    std::string res = needsSemicolon ? getTabs() : "";
    res += varRef->translateToCpp() + " " + assignmentOp + " " + expr->translateToCpp();
    if (needsSemicolon) res += ";";
    return res;
}

std::unique_ptr<IType> AssignmentStmt::getType() const
{
    return varRef->getType()->copy();
}

void AssignmentStmt::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    varRef->setSymbolTable(symTable);
    expr->setSymbolTable(symTable);
}
