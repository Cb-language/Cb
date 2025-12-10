#include "SymbolTable.h"

#include <sstream>

SymbolTable::SymbolTable()
{
    head = std::make_unique<Scope>();
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

void SymbolTable::addVar(const Type& type, const Token& token) const
{
    currScope->addVar(type, token);
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

bool SymbolTable::isLegalCall(const FuncCall& call) const
{
    for (const auto& func : funcs)
    {
        if (call.isLegalCall(func))
        {
            return true;
        }
    }
    return false;
}

void SymbolTable::addFunc(const Func& f)
{
    funcs.push_back(f);
}

void SymbolTable::enterScope()
{
    currScope = currScope->makeNewScope();
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
            return std::make_unique<Func>(func);
        }
    }
    return nullptr;
}

std::string SymbolTable::getFuncsHeaders() const
{
    std::ostringstream oss;
    for (const auto& func : funcs)
    {
        oss << func.translateToCpp() << ";" << std::endl;
    }

    return oss.str();
}
