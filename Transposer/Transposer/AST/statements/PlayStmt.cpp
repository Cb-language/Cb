#include "PlayStmt.h"

PlayStmt::PlayStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, std::vector<std::unique_ptr<Expr>> exprs, const bool printLine)
    : Stmt(token, scope, funcDecl, currClass), exprs(std::move(exprs)) ,printLine(printLine)
{
}

void PlayStmt::analyze() const
{
    for (const auto& var : exprs)
    {
        var->analyze();
    }
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
