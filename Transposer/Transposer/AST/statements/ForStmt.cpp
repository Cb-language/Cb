#include "ForStmt.h"

#include "errorHandling/how/HowDidYouGetHere.h"

ForStmt::ForStmt(const Token& token, IFuncDeclStmt* funcDecl,
                 std::unique_ptr<BodyStmt> body, const bool isIncreasing,
                 std::unique_ptr<Expr> startExpr, std::unique_ptr<Expr> stepExpr, std::unique_ptr<Expr> stopExpr, const std::string& varName, ClassDeclStmt* classDecl)
    : Stmt(token, funcDecl, classDecl), body(std::move(body)), isIncreasing(isIncreasing),
    startExpr(std::move(startExpr)), stepExpr(std::move(stepExpr)), stopExpr(std::move(stopExpr)), varName(varName)
{
}

void ForStmt::analyze() const
{
    // Must have start expr!!
    if (startExpr == nullptr || !startExpr->getType()->isNumberable())
    {
        startExpr->analyze();
    }

    if (stepExpr != nullptr && !stepExpr->getType()->isNumberable())
    {
        stepExpr->analyze();
    }

    if (stopExpr != nullptr && !stopExpr->getType()->isNumberable())
    {
        stopExpr->analyze();
    }

    // Can't really get here, but it's good to check
    if (body == nullptr)
    {
        throw HowDidYouGetHere(token);
    }
    body->analyze();
}

std::string ForStmt::translateToCpp() const
{
    std::ostringstream oss;
    const std::string startStr = startExpr->translateToCpp();
    const std::string stepStr = stepExpr != nullptr ? stepExpr->translateToCpp() : "Primitive<int>(1)";
    const std::string stopStr = stopExpr != nullptr ? stopExpr->translateToCpp() : "Primitive<int>(0)";
    const std::string nameStr = varName;
    std::string stepFullStr;

    if (stepStr == "1")
    {
        stepFullStr = nameStr + (isIncreasing ? "++" : "--");
    }
    else
    {
        stepFullStr = nameStr + (isIncreasing ? " += " : " -= ") + stepStr;
    }

    oss << getTabs() << "for (Primitive<int> " << nameStr << " = " << startStr << "; ";
    oss << nameStr << (isIncreasing ? " < " : " >= ") << stopStr << "; ";
    oss << stepFullStr << ")" << std::endl;
    oss << body->translateToCpp();
    return oss.str();
}
