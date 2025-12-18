#include "SymbolTable.h"

#include <sstream>

#include "AST/statements/expression/FuncCallExpr.h"

SymbolTable::SymbolTable()
{
    head = std::make_unique<Scope>(nullptr, false);
    currScope = head.get();
    currFunc = nullptr; // global is null
}

SymbolTable::~SymbolTable()
{
    funcs.clear();
    head.reset();
    head = nullptr;
    currScope = nullptr;
    currScope = nullptr;
}

std::optional<Var> SymbolTable::getVar(const std::wstring &name) const
{
    return currScope->getVar(name);
}

void SymbolTable::addVar(std::unique_ptr<IType> type, const Token& token) const
{
    currScope->addVar(std::move(type), token);
}

void SymbolTable::addVar(const Var& var, const Token& token) const
{
    currScope->addVar(var, token);
}

bool SymbolTable::doesFuncExist(const Func& f) const
{
    for (const auto& func : funcs)
    {
        if (func == f)
        {
            return true;
        }
    }

    return false;
}

bool SymbolTable::doesFuncExist(const std::wstring& name) const
{
    for (const auto& func : funcs)
    {
        if (func.getFuncName() == name)
        {
            return true;
        }
    }
    return false;
}

bool SymbolTable::isLegalCredit(const FuncCredit& credit) const
{
    for (const auto& func : funcs)
    {
        if (credit.isLegalCredit(func))
        {
            return true;
        }
    }

    return false;
}

std::unique_ptr<IType> SymbolTable::getCallType(FuncCallExpr* expr) const
{
    for (const auto& func : funcs)
    {
        if (expr->isLegalCall(func))
        {
            return func.getType()->copy();
        }
    }

    return nullptr;
}

void SymbolTable::addFunc(const Func& f)
{
    funcs.push_back(f.copy());
}

void SymbolTable::enterScope(bool isBreakable)
{
    currScope = currScope->makeNewScope(isBreakable);
}

void SymbolTable::exitScope()
{
    currScope = currScope->getParent();
}

int SymbolTable::getLevel() const
{
    return currScope->getLevel();
}

void SymbolTable::changeFunc(FuncDeclStmt* funcDecl)
{
    currFunc = funcDecl;
}

Scope* SymbolTable::getCurrScope() const
{
    return currScope;
}

FuncDeclStmt* SymbolTable::getCurrFunc() const
{
    return currFunc;
}

std::unique_ptr<Func> SymbolTable::getFunc(const std::wstring& name) const
{
    if (!doesFuncExist(name))
    {
        return nullptr;
    }
    for (const auto& func : funcs)
    {
        if (func.getFuncName() == name)
        {
            return std::make_unique<Func>(func.copy());
        }
    }
    return nullptr;
}

std::string SymbolTable::getFuncsHeaders() const
{
    std::ostringstream oss;
    for (const auto& func : funcs)
    {
        // convention to note write the main's header
        if (func.getFuncName() != L"prelude")
        {
            oss << func.translateToCpp() << ";" << std::endl;
        }
    }

    return oss.str();
}
