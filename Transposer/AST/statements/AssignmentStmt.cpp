#include "AssignmentStmt.h"


AssignmentStmt::AssignmentStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<VarCallExpr> varExpr, const std::wstring& assignmentOp,
                               std::unique_ptr<Expr> expr) : Stmt(scope, funcDecl), varExpr(std::move(varExpr)), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

bool AssignmentStmt::isLegal() const
{
    if (assignmentOp == L"-=" || assignmentOp == L"*=" || assignmentOp == L"/=" || assignmentOp == L"//=" || assignmentOp == L"%=")
    {
        return varExpr->getType() == L"degree" && expr->getType() == L"degree"; // is numberable
    }

    return varExpr->getType() == expr->getType();
}

std::string AssignmentStmt::translateToCpp() const
{
    return Utils::printTabs() + Utils::wstrToStr(varExpr->getName()) + " "
     + Utils::wstrToStr(assignmentOp) + " " + expr->translateToCpp() + ";";
}
