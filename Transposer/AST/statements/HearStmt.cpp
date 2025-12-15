#include "HearStmt.h"

#include <sstream>

HearStmt::HearStmt(Scope* scope, FuncDeclStmt* funcDecl, const std::vector<std::unique_ptr<VarCallExpr>>& vars) : Stmt(scope, funcDecl)
{
    for (const auto& expr : vars)
    {
        this->vars.push_back(std::make_unique<VarCallExpr>(*expr));
    }
}

bool HearStmt::isLegal() const
{
    for (const auto& var : vars)
    {
        if (!(var->getType()->isPrimitive()))
        {
            return false;
        }
    }

    return true;
}

std::string HearStmt::translateToCpp() const
{
    std::ostringstream oss;

    if (vars.empty())
    {
        oss << getTabs() << "system(\"pause\");";
        return oss.str();
    }

    oss << getTabs() << "std::cin";

    for (const auto& var : vars)
    {
        oss << " >> " << Utils::wstrToStr(var->getName());
    }

    oss << ";";

    return oss.str();
}