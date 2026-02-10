#include "HearStmt.h"

#include <sstream>

#include "errorHandling/semanticErrors/IllegalHear.h"

HearStmt::HearStmt(const Token& token,  Scope* scope, IFuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Call>>& calls) : Stmt(token, scope, funcDecl)
{
    for (auto& call : calls)
    {
        this->calls.push_back(std::move(call));
    }
}

void HearStmt::analyze() const
{
    for (const auto& call : calls)
    {
        if (call->getType()->getArrLevel() > 0 || !(call->getType()->isPrimitive()))
        {
            throw IllegalHear(token, call->getType()->toString());
        }
    }
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