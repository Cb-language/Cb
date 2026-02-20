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

std::string Object::toString() const
{
    return "";
}

std::unique_ptr<Object> Object::clone() const
{
    return std::make_unique<Object>();
}

Object& Object::operator[](int index)
{
    return *this;
}

Object& Object::operator[](Primitive<int> index)
{
    return operator[](index.getValue());
}

Object& Object::operator*()
{
    return *this;
}

Primitive<bool> Object::equals(const Object& other) const
{
    return Primitive<bool>(false);
}

std::ostream& operator<<(std::ostream& os, const Object& obj)
{
    os << obj.toString();
    return os;
}
