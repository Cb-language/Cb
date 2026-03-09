#include "BreakStmt.h"

#include "errorHandling/semanticErrors/StmtNotBreakable.h"

BreakStmt::BreakStmt(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl) : Stmt(token, funcDecl, classDecl)
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
