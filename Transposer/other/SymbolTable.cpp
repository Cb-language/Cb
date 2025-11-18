#include "SymbolTable.h"

SymbolTable::SymbolTable()
{
    head = std::make_unique<Scope>();
    curr = head.get();
}

SymbolTable::~SymbolTable()
{
    head.reset();
    head = nullptr;
    curr = nullptr;
}

std::optional<Var> SymbolTable::getVar(const Var& v) const
{
    return curr->getVar(v);
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
