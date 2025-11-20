#include "AssignmentStmt.h"


AssignmentStmt::AssignmentStmt(const Var& var, const std::wstring& assignmentOp,
                               std::unique_ptr<Expr> expr) : var(var), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

bool AssignmentStmt::isLegal() const
{
    if (assignmentOp == L"-=" || assignmentOp == L"*=" || assignmentOp == L"/=" || assignmentOp == L"//=" || assignmentOp == L"%=")
    {
        return var.getType() == L"degree"; // is numberable
    }

    return var.getType() == expr->getType();
}

std::string AssignmentStmt::translateToCpp() const
{
    return var.getType().translateTypeToCpp() + " " + Utils::wstrToStr(var.getName()) + " "
     + Utils::wstrToStr(assignmentOp) + " " + expr->translateToCpp();
}
