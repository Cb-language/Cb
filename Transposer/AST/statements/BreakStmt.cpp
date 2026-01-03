#include "BreakStmt.h"

BreakStmt::BreakStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl) : Stmt(token, scope, funcDecl)
{
}

bool BreakStmt::isLegal() const
{
    return scope->getIsBreakable();
}

std::string BreakStmt::translateToCpp() const
{
    return getTabs() + "break;";
}
