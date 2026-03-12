#pragma once
#include "Expression.h"

class Call : public Expr
{
protected:
    bool isClassItem = false;
    explicit Call(const Token& token) : Expr(token) {}

public:

    virtual std::string toString() const = 0;

    bool getIsClassItem() const;
    void setIsClassItem(bool value);
};

inline bool Call::getIsClassItem() const
{
    return isClassItem;
}

inline void Call::setIsClassItem(const bool value)
{
    isClassItem = value;
}
