#pragma once

#include "other/Scope.h"

class IFuncDeclStmt;
class ClassDeclStmt;
class SymbolTable;

class Stmt
{
protected:
    const Token token;

    Scope* scope;
    const ClassNode* currClass;
    mutable SymbolTable* symTable;

    explicit Stmt(const Token& token);

    std::string getTabs(const int offset = 0) const;
public:
    virtual ~Stmt() = default;
    virtual void analyze() const = 0;
    virtual std::string translateToCpp() const = 0;
    virtual std::string translateToH() const;

    virtual const Token& getToken() const;
    void setScope(Scope* scope);
    void setClassNode(const ClassNode* currClass);
    void setSymbolTable(SymbolTable* symTable) const;

};

inline Stmt::Stmt(const Token& token) : token(token),
                                        scope(nullptr),
                                        currClass(nullptr),
                                        symTable(nullptr)
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

inline std::string Stmt::translateToH() const
{
    return ""; // not a lot of thing need it
}

inline const Token& Stmt::getToken() const
{
    return token;
}

inline void Stmt::setScope(Scope* scope)
{
    this->scope = scope;
}

inline void Stmt::setClassNode(const ClassNode* currClass)
{
    this->currClass = currClass;
}

inline void Stmt::setSymbolTable(SymbolTable* symTable) const
{
    this->symTable = symTable;
}
