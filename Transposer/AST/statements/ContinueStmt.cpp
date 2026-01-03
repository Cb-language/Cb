#include "ContinueStmt.h"

#include "errorHandling/semanticErrors/StmtNotContinueAble.h"

ContinueStmt::ContinueStmt(const Token& token, Scope *scope, FuncDeclStmt *funcDecl) : Stmt(token, scope, funcDecl)
{
}

void ContinueStmt::analyze() const
{
    if (!scope->getIsContinueAble())
    {
        throw StmtNotContinueAble(token);
    }
}

std::string ContinueStmt::translateToCpp() const
{
    return getTabs() + "continue;";
}
