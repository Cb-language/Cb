#include "PlayStmt.h"

PlayStmt::PlayStmt(Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Expr>> &vars, bool printLine) : Stmt(scope, funcDecl), printLine(printLine)
{
    for (auto& expr : vars)
    {
        this->vars.emplace_back(std::move(expr));
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
        ret += " << " + (var->translateToCpp());
    }
    if (printLine)
    {
        ret += " << std::endl";
    }
    ret += ";";
    return ret;
}
