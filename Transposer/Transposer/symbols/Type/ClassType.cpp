#include "ClassType.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/SymbolTable.h"

ClassType::ClassType(const ClassNode* c) : IType(c != nullptr ? c->getClass().getClassName() : L""), c(c)
{
    if (c == nullptr) throw HowDidYouGetHere(Token());
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

bool ClassType::operator==(const std::wstring& other) const
{
    return *this == ClassType(SymbolTable::getClass(other));
}

bool ClassType::operator!=(const std::wstring& other) const
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

std::wstring ClassType::getType() const
{
    return type;
}

const ClassNode* ClassType::getClass() const
{
    return c;
}

std::string ClassType::translateTypeToCpp() const
{
    return "SafePtr<" + Utils::wstrToStr(type) + ">";
}

std::unique_ptr<IType> ClassType::copy() const
{
    return std::make_unique<ClassType>(c);
}
