#include "PlayStmt.h"

PlayStmt::PlayStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Expr>> exprs, const bool printLine)
    : Stmt(token, scope, funcDecl), exprs(std::move(exprs)) ,printLine(printLine)
{
}

bool PlayStmt::isLegal() const
{
    for (const auto& var : exprs)
    {
        if (!var->isLegal())
        {
            return false;
        }
    }
    return true;
}

std::string PlayStmt::translateToCpp() const
{
    std::string ret = getTabs() + "std::cout";
    for (const auto& var : exprs)
    {
        if (var->getType()->getType() == L"mute")
        {
            ret += " << (" + (var->translateToCpp()) + R"( ? "cres" : "demen"))";
            continue;
        }
        ret += " << " + (var->translateToCpp());
    }
    if (printLine)
    {
        ret += " << std::endl";
    }
    ret += ";";
    return ret;
}
