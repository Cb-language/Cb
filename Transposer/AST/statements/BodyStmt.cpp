#include "BodyStmt.h"
#include "ReturnStmt.h"

BodyStmt::BodyStmt(Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Stmt>>& stmts, const bool isGlobal, const bool hasReturn) :
    Stmt(scope, funcDecl), stmts(std::move(stmts)), isGlobal(isGlobal), hasReturn(hasReturn)
{
}

std::vector<std::unique_ptr<Stmt>>& BodyStmt::getStmts()
{
    return stmts;
}

bool BodyStmt::hasReturnStmt() const
{
    bool foundReturn = false;
    for (const auto& s : stmts)
    {
        if (dynamic_cast<ReturnStmt*>(s.get()) != nullptr) // checking if casting to return stmt works good meaning this is a return stmt
        {
            foundReturn = true;
        }
    }
    return foundReturn;
}

bool BodyStmt::isLegal() const
{
    bool foundReturn = false;
    for (const auto& s : stmts)
    {
        if (!s->isLegal())
        {
            return false;
        }
        if (dynamic_cast<ReturnStmt*>(s.get()) != nullptr) // checking if casting to return stmt works good meaning this is a return stmt
        {
            foundReturn = true;
        }
    }
    if (hasReturn && !foundReturn)
    {
        return false;
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
