#include "FuncCreditStmt.h"

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
}

// no such thing in cpp -> no translating
std::string FuncCreditStmt::translateToCpp() const
{
    return "";
}
