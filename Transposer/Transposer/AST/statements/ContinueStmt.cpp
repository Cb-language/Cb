#include "ContinueStmt.h"
#include "other/SymbolTable.h"
#include "errorHandling/semanticErrors/StmtNotContinueAble.h"

ContinueStmt::ContinueStmt(const Token& token) : Stmt(token)
{
}

void ContinueStmt::analyze() const
{
    if (symTable == nullptr) return;
    if (!symTable->getCurrScope()->getIsContinueAble()) throw StmtNotContinueAble(token);
}

std::string ContinueStmt::translateToCpp() const
{
    return getTabs() + "continue;";
}
