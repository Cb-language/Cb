#pragma once
#include "Expression.h"

class VarReference : public Expr
{
protected:
    bool isClassItem = false;
    explicit VarReference(const Token& token) : Expr(token) {}

public:

    virtual std::string toString() const = 0;

    bool getIsClassItem() const;
    void setIsClassItem(bool value);
};

inline bool VarReference::getIsClassItem() const
{
    return isClassItem;
}

inline void VarReference::setIsClassItem(const bool value)
{
    isClassItem = value;
}
