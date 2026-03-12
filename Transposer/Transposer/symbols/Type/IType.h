#pragma once
#include <string>

#include "other/Utils.h"

class IType
{
public:
    virtual ~IType() = default;
    virtual bool operator==(const IType& other) const = 0;
    virtual bool operator!=(const IType& other) const = 0;

    virtual bool isNumberable() const = 0;
    virtual bool isStringable() const = 0;

    virtual bool isPrimitive() const = 0;

    virtual std::string translateTypeToCpp() const = 0;

    virtual std::unique_ptr<IType> copy() const = 0;

    virtual unsigned int getArrLevel() const {return 0;} // 0 - non array

    virtual std::unique_ptr<IType> getArrType() const {return nullptr;}

    virtual std::string toString() const = 0;
};
