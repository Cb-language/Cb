#include "SymbolTable.h"

SymbolTable::SymbolTable()
{
    head = std::make_unique<Scope>();
    currScope = head.get();
    currFunc = nullptr; // global is null
}

SymbolTable::~SymbolTable()
{
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
