#include "BodyStmt.h"

BodyStmt::BodyStmt(Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Stmt>>& stmts) :
    Stmt(scope, funcDecl)
{
    for (auto& s : stmts)
    {
        this->stmts.push_back(std::move(s));
    }
}

const std::vector<std::unique_ptr<Stmt>>& BodyStmt::getStmts() const
{
    return stmts;
}

bool BodyStmt::isLegal() const
{
    for (const auto& s : stmts)
    {
        if (!s->isLegal())
        {
            return false;
        }
    }
    return true;
}
