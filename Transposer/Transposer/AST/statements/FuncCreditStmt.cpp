#include "FuncCreditStmt.h"

FuncCreditStmt::FuncCreditStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, const FuncCredit& funcCredit)
    : Stmt(token, scope, funcDecl, currClass), funcCredit(funcCredit)
{
}

const std::wstring& FuncCreditStmt::getName() const
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
