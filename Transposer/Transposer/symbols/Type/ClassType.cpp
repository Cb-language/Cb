#include "ClassType.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/SymbolTable.h"

ClassType::ClassType(const std::string& name) : name(name), c(nullptr)
{
}

ClassType::~ClassType()
{
    c = nullptr;
}

bool ClassType::operator==(const IType& other) const
{
    if (const auto otherPtr = dynamic_cast<const ClassType*>(&other)) return c->isDescendantOf(otherPtr->c) || otherPtr->c->isDescendantOf(c);
    return false;
}

bool ClassType::operator!=(const IType& other) const
{
    return !(*this == other);
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

const ClassNode* ClassType::getClassNode() const
{
    return c;
}

void ClassType::setClassNode(const ClassNode& node)
{
    c = &node;
}

std::string ClassType::getName() const
{
    return name;
}

std::string ClassType::translateTypeToCpp() const
{
    return "SafePtr<" + name + ">";
}

std::unique_ptr<IType> ClassType::copy() const
{
    return std::make_unique<ClassType>(name);
}

std::string ClassType::toString() const
{
    return name;
}
