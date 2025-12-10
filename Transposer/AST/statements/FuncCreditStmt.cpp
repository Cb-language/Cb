#include "FuncCreditStmt.h"

FuncCreditStmt::FuncCreditStmt(Scope* scope, FuncDeclStmt* funcDecl, const FuncCredit& funcCredit) : Stmt(scope, funcDecl), funcCredit(funcCredit)
{
}

// checked after the parsing via the credit queue
bool FuncCreditStmt::isLegal() const
{
    return true;
}

// no such thing in cpp -> no translating
std::string FuncCreditStmt::translateToCpp() const
{
    return "";
}
