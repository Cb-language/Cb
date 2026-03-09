#include "FuncCreditStmt.h"

FuncCreditStmt::FuncCreditStmt(const Token& token, IFuncDeclStmt* funcDecl, const FuncCredit& funcCredit, ClassDeclStmt* classDecl)
    : Stmt(token, funcDecl, classDecl), funcCredit(funcCredit)
{
}

const std::string& FuncCreditStmt::getName() const
{
    return funcCredit.getName();
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
