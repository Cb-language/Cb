#include "PlayStmt.h"

PlayStmt::PlayStmt(const Token& token, std::vector<std::unique_ptr<Expr>> exprs, const bool printLine)
    : Stmt(token), exprs(std::move(exprs)) ,printLine(printLine)
{
}

void PlayStmt::analyze() const
{
    if (symTable == nullptr) return;

    for (const auto& var : exprs)
    {
        var->setSymbolTable(symTable);
        var->setScope(scope);
        var->setClassNode(currClass);
        var->analyze();
    }
}

std::string PlayStmt::translateToCpp() const
{
    std::string ret = getTabs() + "std::cout";
    for (const auto& var : exprs)
    {
        if (var->getType()->toString() == "mute")
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
