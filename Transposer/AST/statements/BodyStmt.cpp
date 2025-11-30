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

std::string BodyStmt::translateToCpp() const
{

    std::string out = Utils::printTabs() + "{\n";
    bool first = true;
    for (const auto& s : stmts)
    {
        if (!first)
        {
            out += "\n";
        }
        out += Utils::printTabs() + s->translateToCpp();
        first = false;
    }
    out += "\n" + Utils::printTabs() + "}\n";
    return out;
}
