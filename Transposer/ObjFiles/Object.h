#pragma once
#include <memory>
#include <sstream>

template <typename T>
requires std::is_arithmetic_v<T>
class Primitive;

class Object
{
public:
    Object() = default;
    virtual ~Object() = default;
    Object(const Object&) = default;
    virtual Primitive<bool> operator==(const Object& other) const;
    virtual Primitive<bool> operator!=(const Object& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Object& obj);
    virtual std::string toString(int indent = 0) const;
    virtual std::string toString(Primitive<int> indent) const;
    virtual Object& operator*();

    virtual const Object* get() const;
    virtual const Object* clone() const;

protected:
    virtual Primitive<bool> equals(const Object& other) const;
    static std::string getIndents(int indents);
};
