#include "FuncDeclStmt.h"

FuncDeclStmt::FuncDeclStmt(Scope* scope, const std::wstring& funcName, const Type& returnType,
const std::vector<Var>& args, std::vector<std::unique_ptr<Func>>& credited) : Stmt(scope),
    func(Func(returnType, funcName, args))
{
    for (auto& c : credited)
    {
        this->credited.emplace_back(std::move(c));
    }
    this->setFuncDecl(this);
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
    if (this->funcDecl == nullptr)
    {
        return false; // already in a function
    }
    return true;
}


std::string FuncDeclStmt::translateToCpp() const
{
    return func.translateToCpp();
}
