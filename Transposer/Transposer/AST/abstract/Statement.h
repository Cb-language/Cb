#pragma once

#include <locale>
#include <sstream>

#include "other/Scope.h"
#include "other/Utils.h"

class IFuncDeclStmt;

class Stmt
{
protected:
    const Token token;
    Scope* scope;
    IFuncDeclStmt* funcDecl;
    const ClassNode* currClass;

    Stmt(const Token& token, Scope* scope, const ClassNode* currClass);
    Stmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass);
    std::string getTabs(const int offset = 0) const;
public:
    virtual ~Stmt();
    virtual void analyze() const = 0;
    virtual std::string translateToCpp() const = 0;
    virtual std::string translateToH() const;

    void setFuncDecl(IFuncDeclStmt *funcDecl);
};

inline Stmt::Stmt(const Token& token, Scope* scope, const ClassNode* currClass) : token(token), scope(scope), currClass(currClass)
{
    funcDecl = nullptr;
}

inline Stmt::Stmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass) : token(token), scope(scope), funcDecl(funcDecl), currClass(currClass)
{
}

inline std::string Stmt::getTabs(const int offset) const
{
    const int level = scope->getLevel();
    std::string res;

    for (auto i = 0; i < level + offset; i++)
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

inline std::string Stmt::translateToH() const
{
    return ""; // not a lot of thing need it
}

inline void Stmt::setFuncDecl(IFuncDeclStmt* funcDecl)
{
    this->funcDecl = funcDecl;
}
