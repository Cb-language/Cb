#include "SymbolTable.h"

SymbolTable::SymbolTable()
{
    head = std::make_unique<Scope>();
    curr = head.get();
    funcDeclStack.push(nullptr); // global in null
}

SymbolTable::~SymbolTable()
{
    head.reset();
    head = nullptr;
    curr = nullptr;

    while (funcDeclStack.size() > 0)
    {
        funcDeclStack.pop();
    }
}

std::optional<Var> SymbolTable::getVar(const std::wstring &name) const
{
    return curr->getVar(name);
}

void SymbolTable::addVar(const Type& type, const Token& token) const
{
    curr->addVar(type, token);
}

void SymbolTable::enterScope()
{
    curr = curr->makeNewScope();
}

void SymbolTable::exitScope()
{
    curr = curr->getParent();
}

int SymbolTable::getLevel() const
{
    return curr->getLevel();
}

void SymbolTable::addFunc(FuncDeclStmt* funcDecl)
{
    funcDeclStack.push(funcDecl);
}

void SymbolTable::removeFunc()
{
    if (funcDeclStack.size() > 1)
    {
        funcDeclStack.pop();
    }
}

Scope* SymbolTable::getCurrScope() const
{
    return curr;
}

FuncDeclStmt* SymbolTable::getFuncDecl() const
{
    return funcDeclStack.top();
}
