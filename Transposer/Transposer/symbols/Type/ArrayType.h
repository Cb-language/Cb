#pragma once
#include <memory>

#include "IType.h"

class ArrayType : public IType
{
private:
    std::unique_ptr<IType> valueType;
    unsigned int arrayLevel;

public:
    explicit ArrayType(std::unique_ptr<IType> valueType);

    bool operator==(const IType& other) const override;
    bool operator!=(const IType& other) const override;

    bool isNumberable() const override;
    bool isStringable() const override;

    bool isPrimitive() const override;

    std::string translateTypeToCpp() const override;

    std::unique_ptr<IType> copy() const override;

    unsigned int getArrLevel() const override;

    std::unique_ptr<IType> getArrType() const override;

    std::string toString() const override;
};
