#include "ForStmt.h"

ForStmt::ForStmt(Scope* scope, FuncDeclStmt* funcDecl,
    std::unique_ptr<BodyStmt> body, const bool isIncreasing, std::unique_ptr<Expr> startExpr, std::unique_ptr<Expr> stepExpr, std::unique_ptr<Expr> stopExpr,
    const std::wstring& varName)
    : Stmt(scope, funcDecl), body(std::move(body)), isIncreasing(isIncreasing),
    startExpr(std::move(startExpr)), stepExpr(std::move(stepExpr)), stopExpr(std::move(stopExpr)), varName(varName)
{
}

bool ForStmt::isLegal() const
{
    // Must have start expr!!
    if (startExpr == nullptr || !startExpr->isLegal())
    {
        return false;
    }

    if (stepExpr != nullptr && !stepExpr->isLegal())
    {
        return false;
    }

    if (stopExpr != nullptr && !stopExpr->isLegal())
    {
        return false;
    }
    return body != nullptr && body->isLegal();
}

std::string ForStmt::translateToCpp() const
{
    std::ostringstream oss;
    const std::string startStr = startExpr->translateToCpp();
    const std::string stepStr = stepExpr != nullptr ? stepExpr->translateToCpp() : "1";
    const std::string stopStr = stopExpr != nullptr ? stopExpr->translateToCpp() : "0";
    const std::string nameStr = Utils::wstrToStr(varName);
    std::string stepFullStr = "";

    if (stepStr == "1")
    {
        stepFullStr = nameStr + stepStr + (isIncreasing ? "++" : "--");
    }
    else
    {
        stepFullStr = stepStr + (isIncreasing ? " += " : " -= ") + stepStr;
    }

    oss << getTabs() << "for (int " << nameStr << " = " << startStr << "; ";
    oss << nameStr << (isIncreasing ? " < " : " >= ") << stopStr << "; ";
    oss << stepFullStr << ";)" << std::endl;
    oss << body->translateToCpp();
    return oss.str();
}
