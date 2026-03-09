#include "ContinueStmt.h"

#include "errorHandling/semanticErrors/StmtNotContinueAble.h"

ContinueStmt::ContinueStmt(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl) : Stmt(token, funcDecl, classDecl)
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
