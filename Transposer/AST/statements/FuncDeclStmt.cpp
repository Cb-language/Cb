#include "FuncDeclStmt.h"

FuncDeclStmt::FuncDeclStmt(Scope* scope, const std::wstring& funcName, const Type& returnType,
const std::vector<Var>& args, std::vector<std::unique_ptr<FuncCreditStmt>>& credited) : Stmt(scope),
    func(Func(returnType, funcName, args)), body(nullptr),
    hasReturned(false)
{
    for (auto& c : credited)
    {
        this->credited.emplace_back(std::move(c));
    }
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

const Func& FuncDeclStmt::getFunc() const
{
    return func;
}

void FuncDeclStmt::setBody(std::unique_ptr<BodyStmt> body)
{
    this->body = std::move(body);
}

void FuncDeclStmt::setHasReturned(const bool hasReturned)
{
    this->hasReturned = hasReturned;
}

bool FuncDeclStmt::getHasReturned() const
{
    return hasReturned;
}

bool FuncDeclStmt::isLegal() const
{
    return (func.getType().getType() == L"fermata" || hasReturned) && this->funcDecl == nullptr; // if not nullptr, there is a func inside a func
}


std::string FuncDeclStmt::translateToCpp() const
{
    return func.translateToCpp() + "\n" + body->translateToCpp() + "\n";
}
