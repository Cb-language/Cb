#include "FuncCreditStmt.h"

FuncCreditStmt::FuncCreditStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, const FuncCredit& funcCredit) : Stmt(token, scope, funcDecl), funcCredit(funcCredit)
{
}

const std::wstring& FuncCreditStmt::getName() const
{
    return funcCredit.getName();
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
