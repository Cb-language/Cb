#include "FuncCreditStmt.h"

FuncCreditStmt::FuncCreditStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const FuncCredit& funcCredit)
    : Stmt(token, scope, funcDecl, currClass), funcCredit(funcCredit)
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
