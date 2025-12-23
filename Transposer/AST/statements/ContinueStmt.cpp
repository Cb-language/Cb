#include "ContinueStmt.h"

ContinueStmt::ContinueStmt(Scope *scope, FuncDeclStmt *funcDecl) : Stmt(scope, funcDecl)
{
}

bool ContinueStmt::isLegal() const
{
    return scope->getIsBreakable();
}

std::string ContinueStmt::translateToCpp() const
{
    return getTabs() + "continue;\n";
}
