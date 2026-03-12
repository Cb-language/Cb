#include "FuncCreditStmt.h"
#include "other/SymbolTable.h"
#include "errorHandling/semanticErrors/IllegalCredit.h"

FuncCreditStmt::FuncCreditStmt(const Token& token, const FuncCredit& funcCredit)
    : Stmt(token), funcCredit(funcCredit)
{
}

std::string FuncCreditStmt::getName() const
{
    return translateFQNtoString(funcCredit.getName());
}

// checked after the parsing via the credit queue
void FuncCreditStmt::analyze() const
{
    if (symTable == nullptr) return;
    if (!symTable->isLegalCredit(funcCredit))
    {
        throw IllegalCredit(token, getName());
    }
}

// no such thing in cpp -> no translating
std::string FuncCreditStmt::translateToCpp() const
{
    return "";
}
