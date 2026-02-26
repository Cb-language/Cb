#pragma once
#include <sstream>
#include <vector>
#include "SafePtr.h"
#include "Primitive.h" // you need this for Primitive<T>

class Object;

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
class Array : public Object
{
public:
    Array(); // default constructor
    explicit Array(Primitive<unsigned int> size);
    explicit Array(Primitive<unsigned int> size, SafePtr<T> defaultValue);
    explicit Array(Primitive<unsigned int> size, T defaultValue);
    Array(const Array& other);
    template <typename U>
    explicit Array(const Array<U>& other, bool isValArray = false);
    ~Array() override = default;

    Array& operator=(const Array& other);
    template <typename U>
    Array& operator=(const Array<U>& other);
    Array& operator=(const T& defaultValue);
    template <typename U>
    Array& operator=(const U& defaultValue);

    SafePtr<T>& operator[](Primitive<int> index);
    const SafePtr<T>& operator[](Primitive<int> index) const;

    SafePtr<T>& operator[](int index);
    const SafePtr<T>& operator[](int index) const;

    Primitive<unsigned int> Length() const { return size; }
    Primitive<int> NegLength() const { return Primitive<int>(-size.getValue() - 1); }

    // Only enable add() if T is Object or derived
    template <typename U = T>
    std::enable_if_t<std::is_base_of_v<Object, U>, void>
    add(const U& obj);

    void add(T& value, Primitive<int> index = Primitive<int>(-1));
    void remove(Primitive<int> index);

    Array slice(Primitive<int> start, Primitive<int> stop = Primitive<int>(-1), Primitive<int> step = Primitive<int>(1)) const;

    friend std::ostream& operator<<(std::ostream& os, const Array& other)
    {
        bool first = true;
        for (Primitive<int> i; i < other.size; i++)
        {
            os << (first ? "" : ", ") << "[" << other[i] << "]";
            first = false;
        }
        return os;
    }

    std::string toString() const override;

protected:
    Primitive<bool> equals(const Object& other) const override;

private:
    Primitive<unsigned int> size = Primitive<unsigned int>(0);
    SafePtr<T> defaultValueSet = SafePtr<T>();
    std::vector<SafePtr<T>> data;

    Primitive<int> getNormalIndex(int index) const;
    Primitive<int> getNormalIndex(Primitive<int> index) const;
};

#include "Array.tpp"