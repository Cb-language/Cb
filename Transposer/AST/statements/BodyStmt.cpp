#include "BodyStmt.h"
#include "ReturnStmt.h"

BodyStmt::BodyStmt(Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Stmt>>& stmts, const bool isGlobal) :
    Stmt(scope, funcDecl), stmts(std::move(stmts)), isGlobal(isGlobal)
{
}

std::vector<std::unique_ptr<Stmt>>& BodyStmt::getStmts()
{
    return stmts;
}

bool BodyStmt::isLegal() const
{
    for (const auto& stmt : stmts)
    {
        if (!stmt->isLegal())
        {
            return false;
        }
    }
    return true;
}

std::string BodyStmt::translateToCpp() const
{
    const std::string tabs = getTabs();
    std::string out;

    if (!isGlobal)
    {
        out += tabs + "{\n";
    }

    bool first = true;
    for (const auto& s : stmts)
    {
        if (!first)
        {
            out += "\n";
        }
        out += getTabs() + s->translateToCpp();
        first = false;
    }

    if (!isGlobal)
    {
        out += "\n" + tabs + "}";
    }

    return out;
}
