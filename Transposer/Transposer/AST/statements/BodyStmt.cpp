#include "BodyStmt.h"

#include "BreakStmt.h"
#include "ReturnStmt.h"
#include "other/SymbolTable.h"

BodyStmt::BodyStmt(const Token& token, std::vector<std::unique_ptr<Stmt>>& stmts, const bool isGlobal) :
    Stmt(token), stmts(std::move(stmts)), isGlobal(isGlobal)
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
    if (symTable == nullptr) return;

    if (!isGlobal) symTable->enterScope(isBreakable, isContinueAble);

    for (const auto& stmt : stmts)
    {
        stmt->setSymbolTable(symTable);
        stmt->setScope(symTable->getCurrScope());
        stmt->setClassNode(symTable->getCurrClass());
        stmt->analyze();
    }

    if (!isGlobal) symTable->exitScope();
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

void BodyStmt::setBreakable(const bool isBreakable)
{
    this->isBreakable = isBreakable;
}

void BodyStmt::setContinueAble(const bool isContinueAble)
{
    this->isContinueAble = isContinueAble;
}

void BodyStmt::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    for (const auto& stmt : stmts) stmt->setSymbolTable(symTable);
}
