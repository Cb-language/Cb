#include "AssignmentStmt.h"


AssignmentStmt::AssignmentStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<VarCallExpr> varExpr, const std::wstring& assignmentOp,
                               std::unique_ptr<Expr> expr) : Stmt(scope, funcDecl), varExpr(std::move(varExpr)), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

bool AssignmentStmt::isLegal() const
{
    if (assignmentOp == L"-=" || assignmentOp == L"*=" || assignmentOp == L"/=" || assignmentOp == L"//=" || assignmentOp == L"%=")
    {
        return varExpr->getType()->copy()->isNumberable() && expr->getType()->copy()->isNumberable();
    }

    return *(varExpr->getType()->copy()) == *(expr->getType()->copy());
}

std::string AssignmentStmt::translateToCpp() const
{

    std::string varName = Utils::wstrToStr(varExpr->getName());
    if (assignmentOp == L"+=" || assignmentOp == L"=")
    {
        const bool isVarStr = varExpr->getType()->getType() == L"bar";
        const bool isExprStr = expr->getType()->getType() == L"bar" || expr->getType()->getType() == L"note";

        if (isVarStr && !isExprStr)
        {
            return getTabs() + varName + " " + Utils::wstrToStr(assignmentOp) + " std::to_string(" + expr->translateToCpp() + ");";
        }
    }

    if (assignmentOp == L"//=")
    {
        return getTabs() + varName + " = " + "static_cast<double>(" + varName +") / static_cast<double>(" + expr->translateToCpp() + ");";
    }


    return getTabs() + varName + " "
     + Utils::wstrToStr(assignmentOp) + " " + expr->translateToCpp() + ";";
}
