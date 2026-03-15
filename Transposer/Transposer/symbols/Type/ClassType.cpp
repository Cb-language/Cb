#include "ClassType.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/SymbolTable.h"

ClassTree& ClassType::classTree = ClassTree::instance();

ClassType::ClassType(const FQN& name) : name(name), c(nullptr)
{
}

ClassType::~ClassType()
{
    c = nullptr;
}

bool ClassType::operator==(const IType& other) const
{
    if (translateFQNtoString(other.getFQN()) == translateFQNtoString(name)) return true;

    if (const auto otherPtr = dynamic_cast<const ClassType*>(&other))
    {
        const auto self = classTree.find(name);
        const auto otherNode = classTree.find(otherPtr->getFQN());

        if (self != nullptr && otherNode != nullptr && (self->isDescendantOf(otherNode) || otherNode->isDescendantOf(self))) return true;
    }
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
    return translateFQNtoString(name);
}

std::string ClassType::translateTypeToCpp() const
{
    return "SafePtr<" + translateFQNtoString(name) + ">";
}

std::unique_ptr<IType> ClassType::copy() const
{
    return std::make_unique<ClassType>(name);
}

std::string ClassType::toString() const
{
    return translateFQNtoString(name);
}

FQN ClassType::getFQN() const
{
    return name;
}
