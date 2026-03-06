#include "FuncDeclStmt.h"

#include "errorHandling/semanticErrors/FuncInsideFunc.h"
#include "errorHandling/semanticErrors/NoReturn.h"
#include "../../errorHandling/classErrors/VirtualNonMethod.h"

FuncDeclStmt::FuncDeclStmt(const Token& token, Scope* scope, const ClassNode* currClass,
    const std::wstring& funcName, std::unique_ptr<IType> returnType, const std::vector<Var>& args,
    std::vector<std::unique_ptr<FuncCreditStmt>>& credited, bool isMethod, const VirtualType& virtualType)
        : IFuncDeclStmt(token, scope, currClass), func(Func(std::move(returnType), funcName, args, virtualType)), body(nullptr), hasReturned(false), isMethod(isMethod), virtualType(virtualType)
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

Func FuncDeclStmt::getFunc() const
{
    return func;
}

void FuncDeclStmt::setVirtual(const VirtualType vType)
{
    this->virtualType = vType;
    this->func.setVirtual(vType);
}

VirtualType FuncDeclStmt::getVirtual() const
{
    return virtualType;
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
    if (virtualType != VirtualType::NONE && !isMethod)
    {
        throw VirtualNonMethod(token);
    }

    if (func.getType()->getType() != L"fermata" && !hasReturned && virtualType != VirtualType::PURE)
    {
        throw NoReturn(token);
    }

    if (this->funcDecl != nullptr)
    {
        throw FuncInsideFunc(token);
    }

    if (virtualType != VirtualType::PURE)
    {
        body->analyze();
    }
}


std::string FuncDeclStmt::translateToCpp() const
{
    return func.translateToCpp() + "\n" + body->translateToCpp() + "\n";
}

std::string FuncDeclStmt::translateToH() const
{
    if (func.getFuncName() == L"prelude") return "";

    return func.translateToCpp() + ";";
}

std::string FuncDeclStmt::translateToCppClass(const std::wstring& className) const
{
    return func.translateToCpp(className) + "\n" + body->translateToCpp() + "\n";
}
