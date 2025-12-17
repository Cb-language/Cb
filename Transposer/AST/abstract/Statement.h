#pragma once

#include <locale>
#include "other/Utils.h"

class FuncDeclStmt;

class Stmt
{
protected:
    Scope* scope;
    FuncDeclStmt* funcDecl;
    const bool isBreakable = false;

    explicit Stmt(Scope* scope);
    Stmt(Scope* scope, FuncDeclStmt* funcDecl);
    Stmt(Scope* scope, FuncDeclStmt* funcDecl, const bool isBreakable);
    std::string getTabs() const;
public:
    virtual ~Stmt();
    virtual bool isLegal() const = 0;
    virtual std::string translateToCpp() const = 0;

    void setFuncDecl(FuncDeclStmt *funcDecl);
    const bool getIsBreakable() const;
};

inline Stmt::Stmt(Scope* scope) : scope(scope)
{
    funcDecl = nullptr;
}

inline Stmt::Stmt(Scope* scope, FuncDeclStmt* funcDecl) : scope(scope), funcDecl(funcDecl)
{
}

inline Stmt::Stmt(Scope* scope, FuncDeclStmt* funcDecl, const bool isBreakable) : scope(scope), funcDecl(funcDecl), isBreakable(isBreakable)
{
}

inline std::string Stmt::getTabs() const
{
    const int level = scope->getLevel();
    std::string res;

    for (auto i = 0; i < level; i++)
    {
        res += "\t";
    }

    return res;
}

inline Stmt::~Stmt()
{
    scope = nullptr;
    funcDecl = nullptr;
}

inline void Stmt::setFuncDecl(FuncDeclStmt* funcDecl)
{
    this->funcDecl = funcDecl;
}

inline const bool Stmt::getIsBreakable() const
{
    return isBreakable;
}
