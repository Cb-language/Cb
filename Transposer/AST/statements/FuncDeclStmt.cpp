#include "FuncDeclStmt.h"

FuncDeclStmt::FuncDeclStmt(Scope *scope, const std::wstring &funcName, const Type &returnType, const std::vector<Var> &args) : Stmt(scope),
    func(Func(returnType, funcName, args))
{
    this->setFuncDecl(std::move(funcDecl));
}

const std::vector<Var>& FuncDeclStmt::getArgs() const
{
    return func.getArgs();
}

std::wstring FuncDeclStmt::getName() const
{
    return func.getFuncName();
}

Type FuncDeclStmt::getReturnType() const
{
    return func.getType();
}

bool FuncDeclStmt::isLegal() const
{
    return true;
}


std::string FuncDeclStmt::translateToCpp() const
{
    return func.translateToCpp();
}
