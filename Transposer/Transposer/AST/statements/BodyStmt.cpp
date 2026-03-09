#include "BodyStmt.h"

#include "BreakStmt.h"
#include "ReturnStmt.h"

BodyStmt::BodyStmt(const Token& token, IFuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Stmt>>& stmts, const bool isGlobal, ClassDeclStmt* classDecl) :
    Stmt(token, funcDecl, classDecl), stmts(std::move(stmts)), isGlobal(isGlobal)
{
}

std::vector<std::unique_ptr<Stmt>>& BodyStmt::getStmts()
{
    return stmts;
}

const std::vector<std::unique_ptr<Stmt>>& BodyStmt::getStmts() const
{
    return stmts;
}

void BodyStmt::analyze() const
{
    for (const auto& stmt : stmts)
    {
        stmt->analyze();
    }
}

std::string BodyStmt::translateToCpp() const
{
    const std::string tabs = getTabs();
    std::string out;

    if (!isGlobal && hasBrace)
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
        out += s->translateToCpp();
        first = false;
    }


    if (!isGlobal && hasBrace)
    {
        out += "\n" + tabs + "}";
    }

    return out;
}

void BodyStmt::setHasBrace(const bool hasBrace)
{
    this->hasBrace = hasBrace;
}