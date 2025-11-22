#pragma once
#include <stack>

#include "Scope.h"

class FuncDeclStmt; // TODO: remove this when making it

class SymbolTable
{
private:
    std::unique_ptr<Scope> head;
    Scope* currScope;
    FuncDeclStmt* currFunc;

public:
    SymbolTable();
    ~SymbolTable();

    // std::nullopt when not found
    std::optional<Var> getVar(const std::wstring& name) const;
    void addVar(const Type& type, const Token& token) const;

    void enterScope();
    void exitScope();

    int getLevel() const;

    void changeFunc(FuncDeclStmt* funcDecl);

    Scope* getCurrScope() const;
    FuncDeclStmt* getCurrFunc() const;
};
