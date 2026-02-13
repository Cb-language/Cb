#include "ClassType.h"

#include "errorHandling/how/HowDidYouGetHere.h"

ClassType::ClassType(ClassNode* c) : IType(c != nullptr ? c->getClass().getClassName() : L""), c(c)
{
    if (c == nullptr) throw HowDidYouGetHere(Token());
}

ClassType::~ClassType()
{
    c = nullptr;
}

bool ClassType::operator==(const IType& other) const
{
    return getType() == other.getType();
}

bool ClassType::operator!=(const IType& other) const
{
    return getType() != other.getType();
}

bool ClassType::operator==(const std::wstring& other) const
{
    return getType() == other;
}

bool ClassType::operator!=(const std::wstring& other) const
{
    return getType() != other;
}

bool ClassType::isNumberable() const
{
    return false;
}

bool ClassType::isStringable() const
{
    return true;
}

bool ClassType::isPrimitive() const
{
    return false;
}

std::wstring ClassType::getType() const
{
    return type;
}

std::string ClassType::translateTypeToCpp() const
{
    return Utils::wstrToStr(type);
}

std::unique_ptr<IType> ClassType::copy() const
{
    return std::make_unique<ClassType>(c);
}
