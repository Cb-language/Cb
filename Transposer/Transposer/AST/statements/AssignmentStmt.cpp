#include "AssignmentStmt.h"

#include "errorHandling/semanticErrors/IllegalTypeCast.h"


AssignmentStmt::AssignmentStmt(const Token& token, IFuncDeclStmt* funcDecl,
    std::unique_ptr<Call> call, const std::string& assignmentOp, std::unique_ptr<Expr> expr, ClassDeclStmt* classDecl)
        : Stmt(token, funcDecl, classDecl), call(std::move(call)), assignmentOp(assignmentOp), expr(std::move(expr))
{
}

void AssignmentStmt::analyze() const
{
    if (assignmentOp == "-=" || assignmentOp == "*=" || assignmentOp == "/=" || assignmentOp == "//=" || assignmentOp == "%=")
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
    if (assignmentOp == "+=" || assignmentOp == "=")
    {
        const bool isVarStr = call->getType()->toString() == "bar";

        if (const bool isExprStr = expr->getType()->toString() == "bar" || expr->getType()->toString() == "note"; isVarStr && !isExprStr)
        {
            return getTabs() + varName + " " + assignmentOp + " std::to_string(" + expr->translateToCpp() + ");";
        }
    }

    if (assignmentOp == "//=")
    {
        return getTabs() + varName + " = " + "static_cast<double>(" + varName +") / static_cast<double>(" + expr->translateToCpp() + ");";
    }


    return getTabs() + Utils::removeAllFirstTabs(varName) + " "
     + assignmentOp + " " + expr->translateToCpp() + ";";
}
