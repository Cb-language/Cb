#include "AssignmentStmt.h"

#include "errorHandling/semanticErrors/IllegalTypeCast.h"


AssignmentStmt::AssignmentStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Call> call, const std::wstring& assignmentOp,
                               std::unique_ptr<Expr> expr) : Stmt(token, scope, funcDecl), call(std::move(call)), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

void AssignmentStmt::analyze() const
{
    if (assignmentOp == L"-=" || assignmentOp == L"*=" || assignmentOp == L"/=" || assignmentOp == L"//=" || assignmentOp == L"%=")
    {
        if (!call->getType()->copy()->isNumberable())
        {
            throw IllegalTypeCast(token, call->getType()->toString(), "degree");
        }

        if (!expr->getType()->copy()->isNumberable())
        {
            throw IllegalTypeCast(token, expr->getType()->toString(), "degree");
        }
    }

    if (*(call->getType()->copy()) != *(expr->getType()->copy()))
    {
        throw IllegalTypeCast(token, expr->getType()->toString(), call->getType()->toString());
    }
}

std::string AssignmentStmt::translateToCpp() const
{
    const std::string varName = call->translateToCpp();
    if (assignmentOp == L"+=" || assignmentOp == L"=")
    {
        const bool isVarStr = call->getType()->getType() == L"bar";
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
