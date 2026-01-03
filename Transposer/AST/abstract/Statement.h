#pragma once

#include <locale>
#include <sstream>
#include "other/Utils.h"

class FuncDeclStmt;

class Stmt
{
protected:
    const Token token;
    Scope* scope;
    FuncDeclStmt* funcDecl;

    Stmt(const Token& token, Scope* scope);
    Stmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl);
    std::string getTabs() const;
public:
    virtual ~Stmt();
    virtual void analyze() const = 0;
    virtual std::string translateToCpp() const = 0;

    void setFuncDecl(FuncDeclStmt *funcDecl);
};

inline Stmt::Stmt(const Token& token, Scope* scope) : token(token), scope(scope)
{
    funcDecl = nullptr;
}

inline Stmt::Stmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl) : token(token), scope(scope), funcDecl(funcDecl)
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
