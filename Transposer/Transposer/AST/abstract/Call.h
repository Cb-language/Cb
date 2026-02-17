#pragma once
#include "Expression.h"

class Call : public Expr
{
protected:
    bool isClassItem = false;
    explicit Call(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const bool hasParens)
                    : Expr(token, scope, funcDecl, currClass, hasParens) {}

public:
    Call(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass) : Expr(token, scope, funcDecl, currClass) {}

    virtual std::wstring toString() const = 0;

    bool getIsClassItem() const;
    void setIsClassItem(bool value);
};

inline bool Call::getIsClassItem() const
{
    return isClassItem;
}

inline void Call::setIsClassItem(bool value)
{
    isClassItem = value;
}
