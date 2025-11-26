#include "PlayStmt.h"

PlayStmt::PlayStmt(Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Expr>> &exprs, const bool printLine) : Stmt(scope, funcDecl), printLine(printLine)
{
    for (auto& expr : exprs)
    {
        this->exprs.emplace_back(std::move(expr));
    }
}

bool PlayStmt::isLegal() const
{
    for (const auto& var : exprs)
    {
        if (!(var->getType().isPrimitive()) || !var->isLegal())
        {
            return false;
        }
    }
    return true;
}

std::string PlayStmt::translateToCpp() const
{
    std::string ret = Utils::printTabs() + "std::cout";
    for (const auto& var : exprs)
    {
        ret += " << " + (var->translateToCpp());
    }
    if (printLine)
    {
        ret += " << std::endl";
    }
    ret += ";";
    return ret;
}
