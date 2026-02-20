#include "String.h"

String::String()
= default;

String::String(const std::string& basicString) : value(basicString)
{
}

String::String(const String& string) : value(string.value)
{
}

String::String(const char* basicCharPtr) : value(basicCharPtr)
{
}

std::string String::toString() const
{
    return value;
}

std::unique_ptr<Object> String::clone() const
{
    return std::make_unique<String>(value);
}

String& String::operator=(const String& other)
{
    value = other.value;
    return *this;
}

String& String::operator=(const Object& other)
{
    if (this == &other) return *this;

    if (const String* p = dynamic_cast<const String*>(&other))
    {
        value = p->value;
        return *this;
    }

    throw std::logic_error("Cannot cast");
}

String& String::operator=(const char* basicCharPtr)
{
    value = basicCharPtr;
    return *this;
}

String String::operator+(const char* basicCharPtr) const
{
    return String(value + basicCharPtr);
}

String& String::operator+=(const char* basicCharPtr)
{
    value += basicCharPtr;
    return *this;
}

String String::operator+(const Object& obj) const
{
    return String(value + obj.toString());
}

String& String::operator+=(const Object& obj)
{
    value += obj.toString();
    return *this;
}

Primitive<bool> String::equals(const Object& other) const
{
    if (this == &other) return Primitive<bool>(true);

    if (const String* p = dynamic_cast<const String*>(&other))
    {
        return Primitive<bool>(value == p->value);
    }

    return Primitive<bool>(false);
}

std::ostream& operator<<(std::ostream& os, const String& str)
{
    os << '\"' << str.value << '\"';
    return os;
}
