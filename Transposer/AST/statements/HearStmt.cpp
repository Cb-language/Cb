#include "HearStmt.h"

#include <sstream>

HearStmt::HearStmt(const Token& token,  Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Call>>& calls) : Stmt(token, scope, funcDecl)
{
    for (auto& call : calls)
    {
        this->calls.push_back(std::move(call));
    }
}

bool HearStmt::isLegal() const
{
    for (const auto& call : calls)
    {
        if (call->getType()->getArrLevel() > 0 || !(call->getType()->isPrimitive()))
        {
            return false;
        }
    }

    return true;
}

std::string HearStmt::translateToCpp() const
{
    std::ostringstream oss;

    if (calls.empty())
    {
        oss << getTabs() << "system(\"pause\");";
        return oss.str();
    }

    oss << getTabs() << "std::cin";

    for (const auto& call : calls)
    {
        oss << " >> " << call->translateToCpp();
    }

    oss << ";";

    return oss.str();
}