#include "ArrayType.h"

ArrayType::ArrayType(std::unique_ptr<IType> valueType) : IType(L"riff"),  valueType(std::move(valueType))
{
    arrayLevel = this->valueType->getArrLevel() + 1;
}

bool ArrayType::operator==(const IType& other) const
{
    if (arrayLevel != other.getArrLevel())
    {
        return false;
    }

    const auto valType = other.getArrType();

    if (!valType)
    {
        return false;
    }

    return *valueType == *(valType);
}

bool ArrayType::operator!=(const IType& other) const
{
    return !(*this == other);
}

bool ArrayType::operator==(const std::wstring& other) const
{
    if (!other.starts_with(L"riff "))
    {
        return false;
    }

    return *this == other.substr(0, 5);
}

bool ArrayType::operator!=(const std::wstring& other) const
{
    return !(*this == other);
}

bool ArrayType::isNumberable() const
{
    return false;
}

bool ArrayType::isStringable() const
{
    return false;
}

bool ArrayType::isPrimitive() const
{
    return false;
}

std::wstring ArrayType::getType() const
{
    return L"riff " + valueType->getType();
}

std::string ArrayType::translateTypeToCpp() const
{
    return "Array<" + valueType->translateTypeToCpp() + ">";
}

std::unique_ptr<IType> ArrayType::copy() const
{
    return std::make_unique<ArrayType>(valueType->copy());
}

unsigned int ArrayType::getArrLevel() const
{
    return arrayLevel;
}

std::unique_ptr<IType> ArrayType::getArrType() const
{
    return valueType->copy();
}
