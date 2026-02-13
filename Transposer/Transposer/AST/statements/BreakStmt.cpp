#include "BreakStmt.h"

#include "errorHandling/semanticErrors/StmtNotBreakable.h"

BreakStmt::BreakStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass) : Stmt(token, scope, funcDecl, currClass)
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
