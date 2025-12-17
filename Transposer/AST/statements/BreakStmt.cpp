#include "BreakStmt.h"

BreakStmt::BreakStmt(Scope* scope, FuncDeclStmt* funcDecl) : Stmt(scope, funcDecl)
{
}

bool BreakStmt::isLegal() const
{
    return true; // always true
}

std::string BreakStmt::translateToCpp() const
{
    return getTabs() + "break\n";
}
