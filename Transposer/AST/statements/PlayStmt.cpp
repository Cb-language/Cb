#include "PlayStmt.h"

PlayStmt::PlayStmt(const std::vector<std::unique_ptr<ConstValueExpr>> &vars)
{
    for (const auto& expr : vars)
    {
        this->vars.push_back(std::make_unique<ConstValueExpr>(*expr));
    }
}

bool PlayStmt::isLegal() const
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

std::string PlayStmt::translateToCpp() const
{
    std::string ret = Utils::printTabs() + "std::cout ";
    for (const auto& var : vars)
    {
        ret += " << " + Utils::wstrToStr(var->getValue());
    }
    ret += ";";
    return ret;
}
