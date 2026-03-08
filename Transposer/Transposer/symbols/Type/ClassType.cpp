#include "ClassType.h"

#include "errorHandling/how/HowDidYouGetHere.h"

ClassType::ClassType(const ClassNode* c) : IType(c != nullptr ? c->getClass().getClassName() : ""), c(c)
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

bool ClassType::operator==(const std::string& other) const
{
    return getType() == other;
}

bool ClassType::operator!=(const std::string& other) const
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

std::string ClassType::getType() const
{
    return type;
}

const ClassNode* ClassType::getClass() const
{
    return c;
}

std::string ClassType::translateTypeToCpp() const
{
    return "SafePtr<" + type + ">";
}

std::unique_ptr<IType> ClassType::copy() const
{
    return std::make_unique<ClassType>(c);
}
