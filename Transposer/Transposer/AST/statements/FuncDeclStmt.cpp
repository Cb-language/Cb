#include "FuncDeclStmt.h"

#include "errorHandling/semanticErrors/FuncInsideFunc.h"
#include "errorHandling/semanticErrors/NoReturn.h"

FuncDeclStmt::FuncDeclStmt(const Token& token, Scope* scope, const std::wstring& funcName, std::unique_ptr<IType> returnType,
                           const std::vector<Var>& args, std::vector<std::unique_ptr<FuncCreditStmt>>& credited) : Stmt(token, scope),
                                                                                                                   func(Func(std::move(returnType), funcName, args)), body(nullptr),
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

std::unique_ptr<IType> FuncDeclStmt::getReturnType() const
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

const std::vector<std::unique_ptr<FuncCreditStmt>>& FuncDeclStmt::getCredited() const
{
    return credited;
}

void FuncDeclStmt::analyze() const
{
    if (func.getType()->getType() != L"fermata" && !hasReturned)
    {
        throw NoReturn(token);
    }

    if (this->funcDecl != nullptr)
    {
        throw FuncInsideFunc(token);
    }

    body->analyze();
}


std::string FuncDeclStmt::translateToCpp() const
{
    return func.translateToCpp() + "\n" + body->translateToCpp() + "\n";
}

std::string FuncDeclStmt::translateToCppClass(const std::wstring& className) const
{
    return func.translateToCpp(className) + "\n" + body->translateToCpp() + "\n";
}
