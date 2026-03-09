#include "PlayStmt.h"

PlayStmt::PlayStmt(const Token& token, IFuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Expr>> exprs, const bool printLine, ClassDeclStmt* classDecl)
    : Stmt(token, funcDecl, classDecl), exprs(std::move(exprs)) ,printLine(printLine)
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
        if (var->getType()->getType() == "mute")
        {
            ret += " << (" + Utils::removeAllFirstTabs(var->translateToCpp()) + R"( ? "cres" : "demen"))";
            continue;
        }
        ret += " << " + Utils::removeAllFirstTabs(var->translateToCpp());
    }
    if (printLine)
    {
        ret += " << std::end";
    }
    ret += ";";
    return ret;
}
