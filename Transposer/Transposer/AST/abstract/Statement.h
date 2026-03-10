#pragma once

#include "other/Scope.h"

class IFuncDeclStmt;
class ClassDeclStmt;

class Stmt
{
protected:
    const Token token;
    IFuncDeclStmt* funcDecl;
    ClassDeclStmt* classDecl;

    Scope* scope;
    const ClassNode* currClass;

    explicit Stmt(const Token& token);
    explicit Stmt(const Token& token, ClassDeclStmt* classDecl = nullptr);
    Stmt(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl);

    std::string getTabs(const int offset = 0) const;
public:
    virtual ~Stmt();
    virtual void analyze() const = 0;
    virtual std::string translateToCpp() const = 0;
    virtual std::string translateToH() const;

    const ClassNode* getCurrClass() const;
    virtual const Token& getToken() const;
    void setFuncDecl(IFuncDeclStmt *funcDecl);
    void setScope(Scope* scope);
    void setClassNode(const ClassNode* currClass);

};

inline Stmt::Stmt(const Token& token) : token(token),
                                        funcDecl(nullptr),
                                        classDecl(nullptr),
                                        scope(nullptr),
                                        currClass(nullptr)
{
}

inline Stmt::Stmt(const Token& token, ClassDeclStmt* classDecl) : token(token),
                                                                                           funcDecl(nullptr),
                                                                                           classDecl(classDecl),
                                                                                           scope(nullptr),
                                                                                           currClass(nullptr)
{
}

inline Stmt::Stmt(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl) : token(token),
                                                                                           funcDecl(funcDecl),
                                                                                           classDecl(classDecl),
                                                                                           scope(nullptr),
                                                                                           currClass(nullptr)
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

inline const ClassNode* Stmt::getCurrClass() const
{
    return currClass;
}

inline const Token& Stmt::getToken() const
{
    return token;
}

inline void Stmt::setFuncDecl(IFuncDeclStmt* funcDecl)
{
    this->funcDecl = funcDecl;
}

inline void Stmt::setScope(Scope* scope)
{
    this->scope = scope;
}

inline void Stmt::setClassNode(const ClassNode* currClass)
{
    this->currClass = currClass;
}
