#include "HearStmt.h"

#include <sstream>

HearStmt::HearStmt(const std::vector<std::unique_ptr<VarCallExpr>>& vars)
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
        if (!(var->getType().isPrimitive()))
        {
            return false;
        }
    }

    return true;
}

std::string HearStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << Utils::printTabs() << "std::cin";

    for (const auto& var : vars)
    {
        oss << " >> " << Utils::wstrToStr(var->getName());
    }

    oss << ";";

    return oss.str();
}