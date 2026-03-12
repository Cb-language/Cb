#include "BreakStmt.h"
#include "other/SymbolTable.h"
#include "errorHandling/semanticErrors/StmtNotBreakable.h"

BreakStmt::BreakStmt(const Token& token) : Stmt(token)
{
}

void BreakStmt::analyze() const
{
    if (symTable == nullptr) return;
    if (!symTable->getCurrScope()->getIsBreakable()) throw StmtNotBreakable(token);
}

std::string BreakStmt::translateToCpp() const
{
    return getTabs() + "break;";
}
