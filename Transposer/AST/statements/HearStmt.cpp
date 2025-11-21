#include "HearStmt.h"

#include <sstream>

HearStmt::HearStmt(const std::vector<Var>& vars) : vars(vars)
{
}

bool HearStmt::isLegal() const
{
    return true;
}

std::string HearStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << Utils::printTabs() << "std::cin";

    for (const auto& var : vars)
    {
        oss << " >> " << Utils::wstrToStr(var.getName());
    }

    oss << ";" << std::endl;

    return oss.str();
}