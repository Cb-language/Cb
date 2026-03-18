#include "Object.h"
#include "Primitive.h"

Primitive<bool> Object::operator==(const Object& other) const
{
    return Primitive<bool>(this == &other || equals(other));
}

Primitive<bool> Object::operator!=(const Object& other) const
{
    return Primitive<bool>(!(*this == other));
}

std::string Object::toString(int indent) const
{
    return "";
}

std::string Object::toString(Primitive<int> indent) const
{
    return toString(indent.getValue());
}

Object& Object::operator*()
{
    return *this;
}

const Object* Object::get() const
{
    return this;
}

Primitive<bool> Object::equals(const Object& other) const
{
    return Primitive<bool>(true);
}

std::string Object::getIndents(int indents)
{
    return std::string(indents, ' ');
}

std::ostream& operator<<(std::ostream& os, const Object& obj)
{
    os << obj.toString();
    return os;
}
