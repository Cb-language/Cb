#pragma once
#include "Expression.h"

class Call : public Expr
{
protected:
    bool isClassItem = false;
    Call(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl = nullptr)
                    : Expr(token, funcDecl, classDecl) {}

public:

    virtual std::string toString() const = 0;

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
