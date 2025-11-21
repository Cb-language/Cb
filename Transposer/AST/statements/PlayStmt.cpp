#include "PlayStmt.h"

PlayStmt::PlayStmt(const std::vector<std::unique_ptr<VarCallExpr>> &vars, const bool newline) : newline(newline)
{
    for (const auto& expr : vars)
    {
        this->vars.push_back(std::make_unique<VarCallExpr>(*expr));
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
    std::string ret = Utils::printTabs() + "std::cout";
    for (const auto& var : vars)
    {
        ret += " << " + Utils::wstrToStr(var->getName());

        if (var->getType().getType() == L"mute")
        {
            ret += " ? cres : demen";
        }
    }

    if (newline)
    {
        ret += " << std::endl";
    }

    ret += ";";
    return ret;
}
