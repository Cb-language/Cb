#include "ContinueStmt.h"

ContinueStmt::ContinueStmt(const Token& token, Scope *scope, FuncDeclStmt *funcDecl) : Stmt(token, scope, funcDecl)
{
}

bool ContinueStmt::isLegal() const
{
    return scope->getIsBreakable();
}

std::string ContinueStmt::translateToCpp() const
{
    return getTabs() + "continue;";
}
