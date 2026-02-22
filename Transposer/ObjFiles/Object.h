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
    virtual std::string toString() const;
    virtual std::unique_ptr<Object> clone() const;

    virtual Object& operator[](int index);
    virtual Object& operator[](Primitive<int> index);
    virtual Object& operator*();

protected:
    virtual Primitive<bool> equals(const Object& other) const;
};
