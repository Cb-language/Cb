#pragma once

#include <locale>
#include "other/Utils.h"

class FuncDeclStmt; // TODO: remove this when making it

class Stmt
{
protected:
    Scope* scope;
    FuncDeclStmt* funcDecl;

    Stmt(Scope* scope, FuncDeclStmt* funcDecl);
public:
    virtual ~Stmt();
    virtual bool isLegal() const = 0;
    virtual std::string translateToCpp() const = 0;
};

inline Stmt::Stmt(Scope* scope, FuncDeclStmt* funcDecl) : scope(scope), funcDecl(funcDecl)
{
}

inline Stmt::~Stmt()
{
    scope = nullptr;
    funcDecl = nullptr;
}
