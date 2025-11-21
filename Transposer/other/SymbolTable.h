#pragma once
#include "Scope.h"

class SymbolTable
{
private:
    std::unique_ptr<Scope> head;
    Scope* curr;

public:
    SymbolTable();
    ~SymbolTable();

    // std::nullopt when not found
    std::optional<Var> getVar(const std::wstring& name) const;
    void addVar(const Type& type, const Token& token) const;

    void enterScope();
    void exitScope();

    int getLevel() const;
};
