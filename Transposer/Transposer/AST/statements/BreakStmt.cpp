#include "BreakStmt.h"

#include "errorHandling/semanticErrors/StmtNotBreakable.h"

BreakStmt::BreakStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl) : Stmt(token, scope, funcDecl)
{
}

void BreakStmt::analyze() const
{
    if (!scope->getIsBreakable()) throw StmtNotBreakable(token);
}

std::string BreakStmt::translateToCpp() const
{
    return getTabs() + "break;";
}
