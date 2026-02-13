#include "ContinueStmt.h"

#include "errorHandling/semanticErrors/StmtNotContinueAble.h"

ContinueStmt::ContinueStmt(const Token& token, Scope *scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass) : Stmt(token, scope, funcDecl, currClass)
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
