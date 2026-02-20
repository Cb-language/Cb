#include "LibHelper.h"

const std::vector<std::pair<std::filesystem::path, std::string>> LibHelper::files =
{
    { R"(Array.h)", R"(#pragma once
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
    // If T is arithmetic, wrap it in Primitive<T>, else leave T as-is
    using SafeType = std::conditional_t<std::is_arithmetic_v<T>, Primitive<T>, T>;
    using SP = SafePtr<SafeType>;
    Array(); // default constructor
    explicit Array(Primitive<unsigned int> size);
    explicit Array(Primitive<unsigned int> size, SP defaultValue);
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

    SP& operator[](Primitive<int> index) override;
    const SP& operator[](Primitive<int> index) const;

    SP& operator[](int index) override;
    const SP& operator[](int index) const;

    Primitive<unsigned int> Length() const { return size; }
    Primitive<int> NegLength() const { return Primitive<int>(-size.getValue() - 1); }

    // Only enable add() if SafeType is Object or derived
    template <typename U = SafeType>
    std::enable_if_t<std::is_base_of_v<Object, U>, void>
    add(const U& obj);

    void add(SafeType& value, Primitive<int> index = Primitive<int>(-1));
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
    std::unique_ptr<Object> clone() const override;

protected:
    Primitive<bool> equals(const Object& other) const override;

private:
    Primitive<unsigned int> size = Primitive<unsigned int>(0);
    SP defaultValueSet = SP();
    std::vector<SP> data;

    Primitive<int> getNormalIndex(int index) const;
    Primitive<int> getNormalIndex(Primitive<int> index) const;
};

#include "Array.tpp")" },
    { R"(Array.tpp)", R"(#pragma once
#include "Array.h"
#include "Object.h"
#include "String.h"

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array()
{
    size = 1;
    defaultValueSet = SafePtr<SafeType>();
    data.push_back(defaultValueSet.cloneSafePtr());
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(Primitive<unsigned int> size) : Array(size, SafePtr<SafeType>())
{
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(Primitive<unsigned int> size, SafePtr<SafeType> defaultValue)
    : size(size)
{
    defaultValueSet = defaultValue;

    data.reserve(size.getValue());
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
    }
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(Primitive<unsigned int> size, T defaultValue) : Array<T>(size, SafePtr<SafeType>(SafeType(defaultValue)))
{
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(const Array<T>& other)
{
    size = other.size;
    defaultValueSet = other.defaultValueSet;
    data.reserve(size.getValue());
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(SafePtr<SafeType>(other.data[i.getValue()].cloneSafePtr()));
    }
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
Array<T>::Array(const Array<U>& other, bool isValArray)
{
    size = other.Length();
    data.reserve(size);

    if constexpr (std::is_same_v<T, std::string>)
    {
        std::ostringstream oss;
        oss << other[0]; // just for default
        defaultValueSet = SafePtr<String>(String(oss.str()));

        for (Primitive<int> i; i < size; ++i)
        {
            std::ostringstream ossData;
            ossData << other[i];
            data.push_back(SafePtr<String>(String(ossData.str())));
        }
    }
    else
    {
        defaultValueSet = SafePtr<SafeType>();
        for (Primitive<int> i; i < size; ++i)
        {
            data.push_back(SafePtr<SafeType>(static_cast<T>(*other[i])));
        }
    }
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>& Array<T>::operator=(const Array<T>& other)
{
    if (this == &other)
        return *this;

    size = other.size;
    defaultValueSet = other.defaultValueSet;

    data.clear();
    data.reserve(size);
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(other.data[i].cloneSafePtr());
    }
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
Array<T>& Array<T>::operator=(const Array<U>& other)
{
    size = other.Length();
    data.clear();
    data.reserve(size);

    for (Primitive<int> i; i < size; ++i)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            std::ostringstream oss;
            oss << *other[i];
            data.push_back(SafePtr<String>(String(oss.str())));
        }
        else
        {
            data.push_back(SafePtr<SafeType>(static_cast<T>(*other[i])));
        }
    }

    defaultValueSet = data[0].clonePtr();
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>& Array<T>::operator=(const T& defaultValue)
{
    data.clear();
    data.reserve(size);

    defaultValueSet = SafePtr<SafeType>(defaultValue);
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(defaultValueSet.clone());
    }
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
Array<T>& Array<T>::operator=(const U& defaultValue)
{
    data.clear();
    data.reserve(size);

    if constexpr (std::is_same_v<T, std::string>)
    {
        std::ostringstream oss;
        oss << defaultValue;
        data.push_back(SafePtr<String>(String(oss.str())));
    }
    else
    {
        defaultValueSet = SafePtr<SafeType>(static_cast<T>(defaultValue));
    }

    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
    }

    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<typename Array<T>::SafeType>& Array<T>::operator[](Primitive<int> index)
{
    index = getNormalIndex(index);
    return data[index.getValue()];
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
const SafePtr<typename Array<T>::SafeType>& Array<T>::operator[](Primitive<int> index) const
{
    index = getNormalIndex(index);
    return data[index.getValue()];
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
typename Array<T>::SP& Array<T>::operator[](int index)
{
    int i = getNormalIndex(index).getValue();
    return data[i];
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
const typename Array<T>::SP& Array<T>::operator[](int index) const
{
    index = getNormalIndex(index);
    return data[index];
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
std::enable_if_t<std::is_base_of_v<Object, U>, void> Array<T>::add(const U& obj)
{
    data.push_back(SafePtr<SafeType>(obj.clone()));
    ++size;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
void Array<T>::add(SafeType& value, Primitive<int> index)
{
    if (index == Primitive<int> (-1) || index >= size)
        index = size;

    data.insert(data.begin() + index, SafePtr<SafeType>(value));
    ++size;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
void Array<T>::remove(Primitive<int> index)
{
    index = getNormalIndex(index);
    data.erase(data.begin() + index.getValue());
    --size;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T> Array<T>::slice(Primitive<int> start, Primitive<int> stop, Primitive<int> step) const
{
    if (step == Primitive<int>(0))
        throw std::out_of_range("Step cannot be zero");

    int sz = static_cast<int>(size.getValue());
    if (start < 0) start += sz;
    if (stop < 0) stop += sz;
    if (stop == Primitive<int>(-1)) stop = sz;

    Array<T> result;
    result.data.clear();

    for (Primitive<int> i = start; (step > 0 ? i < stop : i > stop); i += step)
    {
        result.data.push_back(data[i].clone());
        ++result.size;
    }

    result.defaultValueSet = defaultValueSet.clone();
    return result;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::string Array<T>::toString() const
{
    std::string str = "[";
    bool first = true;

    for (Primitive<int> i; i < size; ++i)
    {
        if (!first) str += ", ";
        str += data[i.getValue()]->toString();
        first = false;
    }

    str += "]";
    return str;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::unique_ptr<Object> Array<T>::clone() const
{
    return std::make_unique<Array<T>>(*this);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Primitive<bool> Array<T>::equals(const Object& other) const
{
    if (this == &other) return Primitive<bool>(true);

    if (const Array<T>* otherArray = dynamic_cast<const Array<T>* >(&other))
    {
        if (otherArray->size != this->size) return Primitive<bool>(false);

        for (Primitive<int> i; i < otherArray->size; ++i)
        {
            if (*otherArray->data[i.getValue()] != *this->data[i.getValue()]) return Primitive<bool>(false);
        }
        return Primitive<bool>(true);
    }

    return Primitive<bool>(false);
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Primitive<int> Array<T>::getNormalIndex(int index) const
{
    return getNormalIndex(Primitive<int>(index));
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Primitive<int> Array<T>::getNormalIndex(Primitive<int> index) const
{
    int sz = static_cast<int>(size.getValue());
    if (index < 0) index += sz;
    if (index < 0 || index >= sz)
        throw std::out_of_range("Index out of bounds");
    return index;
}
)" },
    { R"(Object.cpp)", R"(#include "Object.h"
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
)" },
    { R"(Object.h)", R"(#pragma once
#include <memory>
#include <sstream>

template <typename T>
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
)" },
    { R"(Primitive.h)", R"(#pragma once

class Object;

class PrimitiveBase : public Object
{
public:
    virtual double getNumericValue() const = 0;
};

template <typename T>
class Primitive : public PrimitiveBase
{
private:
    T value;

public:
    double getNumericValue() const override;
    explicit Primitive(const T& value = T());
    Primitive(const Primitive& other);
    Primitive& operator=(const Primitive& other);

    // ReSharper disable once CppEnforceOverridingFunctionStyle
    Primitive& operator=(const T& other);

    template <typename U>
    Primitive& operator=(const Primitive<U>& other);

    template <typename U>
    Primitive& operator=(const U& other);

    Primitive<T>& operator=(const Object& other);

    std::string toString() const override;
    std::unique_ptr<Object> clone() const override;
    T getValue() const;

    Primitive operator+(const Primitive& other) const;
    Primitive operator-(const Primitive& other) const;
    Primitive operator*(const Primitive& other) const;
    Primitive operator/(const Primitive& other) const;
    Primitive operator%(const Primitive& other) const;
    Primitive& operator++();
    Primitive& operator--();
    Primitive operator++(int);
    Primitive operator--(int);
    Primitive operator-() const;

    Primitive& operator+=(const Primitive& other);
    Primitive& operator-=(const Primitive& other);
    Primitive& operator*=(const Primitive& other);
    Primitive& operator/=(const Primitive& other);
    Primitive& operator%=(const Primitive& other);


    Primitive operator+(const T& other) const;
    Primitive operator-(const T& other) const;
    Primitive operator*(const T& other) const;
    Primitive operator/(const T& other) const;
    Primitive operator%(const T& other) const;

    Primitive& operator+=(const T& other);
    Primitive& operator-=(const T& other);
    Primitive& operator*=(const T& other);
    Primitive& operator/=(const T& other);
    Primitive& operator%=(const T& other);

    template <typename U>
    Primitive operator+(const Primitive<U>& other) const;
    template <typename U>
    Primitive operator-(const Primitive<U>& other) const;
    template <typename U>
    Primitive operator*(const Primitive<U>& other) const;
    template <typename U>
    Primitive operator/(const Primitive<U>& other) const;
    template <typename U>
    Primitive operator%(const Primitive<U>& other) const;

    template <typename U>
    Primitive& operator+=(const Primitive<U>& other);
    template <typename U>
    Primitive& operator-=(const Primitive<U>& other);
    template <typename U>
    Primitive& operator*=(const Primitive<U>& other);
    template <typename U>
    Primitive& operator/=(const Primitive<U>& other);
    template <typename U>
    Primitive& operator%=(const Primitive<U>& other);


    template <typename U>
    Primitive operator+(const U& other) const;
    template <typename U>
    Primitive operator-(const U& other) const;
    template <typename U>
    Primitive operator*(const U& other) const;
    template <typename U>
    Primitive operator/(const U& other) const;
    template <typename U>
    Primitive operator%(const U& other) const;

    template <typename U>
    Primitive& operator+=(const U& other);
    template <typename U>
    Primitive& operator-=(const U& other);
    template <typename U>
    Primitive& operator*=(const U& other);
    template <typename U>
    Primitive& operator/=(const U& other);
    template <typename U>
    Primitive& operator%=(const U& other);

    // Comparison operators: Primitive<T> vs Primitive<T>
    Primitive<bool> operator>(const Primitive<T>& other) const;
    Primitive<bool> operator>=(const Primitive<T>& other) const;
    Primitive<bool> operator<(const Primitive<T>& other) const;
    Primitive<bool> operator<=(const Primitive<T>& other) const;

    // Primitive<T> vs T
    Primitive<bool> operator>(const T& other) const;
    Primitive<bool> operator>=(const T& other) const;
    Primitive<bool> operator<(const T& other) const;
    Primitive<bool> operator<=(const T& other) const;

    // Primitive<T> vs Primitive<U>
    template <typename U>
    Primitive<bool> operator>(const Primitive<U>& other) const;

    template <typename U>
    Primitive<bool> operator>=(const Primitive<U>& other) const;

    template <typename U>
    Primitive<bool> operator<(const Primitive<U>& other) const;

    template <typename U>
    Primitive<bool> operator<=(const Primitive<U>& other) const;

    // Primitive<T> vs U
    template <typename U>
    Primitive<bool> operator>(const U& other) const;

    template <typename U>
    Primitive<bool> operator>=(const U& other) const;

    template <typename U>
    Primitive<bool> operator<(const U& other) const;

    template <typename U>
    Primitive<bool> operator<=(const U& other) const;

    template <typename U>
    Primitive<bool> operator==(const Primitive<U>& other) const;

    template <typename U>
    Primitive<bool> operator!=(const Primitive<U>& other) const;

    // Raw T
    Primitive<bool> operator==(const T& other) const;
    Primitive<bool> operator!=(const T& other) const;

    // Raw U
    template <typename U>
    Primitive<bool> operator==(const U& other) const;

    template <typename U>
    Primitive<bool> operator!=(const U& other) const;

    Primitive<bool> operator||(const Primitive<bool>& other);
    Primitive<bool> operator&&(const Primitive<bool>& other);
    Primitive<bool> operator||(bool other);
    Primitive<bool> operator&&(bool other);
    explicit operator bool() const;

protected:
    Primitive<bool> equals(const Object& other) const override;
};

#include "Primitive.tpp"
)" },
    { R"(Primitive.tpp)", R"(#pragma once
#include "Primitive.h"

template <typename T>
double Primitive<T>::getNumericValue() const
{
    return static_cast<double>(value);
}

template <typename T>
Primitive<T>::Primitive(const T& value) : value(value)
{
}

template <typename T>
Primitive<T>::Primitive(const Primitive& other) : value(other.value)
{
}

template <typename T>
Primitive<T>& Primitive<T>::operator=(const Primitive& other)
{
    if (this == &other) return *this;

    value = other.value;

    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator=(const T& other)
{
    value = other;

    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator=(const Primitive<U>& other)
{
    value = static_cast<T>(other.getValue());

    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator=(const U& other)
{
    static_cast<T>(other);

    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator=(const Object& other)
{
    if (this == &other) return *this;

    if (const Primitive<T>* p = dynamic_cast<const Primitive<T>*>(&other))
    {
        value = p->value;
    }
    else
    {
        throw std::logic_error("Cannot cast");
    }

    return *this;
}

template <typename T>
Primitive<bool> Primitive<T>::equals(const Object& other) const
{
    if (this == &other) return Primitive<bool>(true);

    if (const PrimitiveBase* pBase = dynamic_cast<const PrimitiveBase*>(&other))
    {
        return Primitive<bool>(getNumericValue() == pBase->getNumericValue());
    }

    return Primitive<bool>(false);
}

// ------ non-member func for T op Primitive
template <typename T>
Primitive<T> operator+(T left, const Primitive<T>& right)
{
    return Primitive(left) + right;
}

template <typename T>
Primitive<T> operator-(T left, const Primitive<T>& right)
{
    return Primitive(left) - right;
}

template <typename T>
Primitive<T> operator*(T left, const Primitive<T>& right)
{
    return Primitive(left) * right;
}

template <typename T>
Primitive<T> operator/(T left, const Primitive<T>& right)
{
    return Primitive(left) / right;
}

template <typename T>
Primitive<T> operator%(T left, const Primitive<T>& right)
{
    return Primitive(left) % right;
}

template <typename T, typename U>
Primitive<T> operator+(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) + right;
}

template <typename T, typename U>
Primitive<T> operator-(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) - right;
}

template <typename T, typename U>
Primitive<T> operator*(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) * right;
}

template <typename T, typename U>
Primitive<T> operator/(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) / right;
}

template <typename T, typename U>
Primitive<T> operator%(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) % right;
}

// -------------

template <typename T>
std::string Primitive<T>::toString() const
{
    if constexpr (std::is_same_v<T,bool>) return value ? "cres" : "demen";
    return std::to_string(value);
}

template <typename T>
std::unique_ptr<Object> Primitive<T>::clone() const
{
    return std::make_unique<Primitive>(value);
}

template <typename T>
T Primitive<T>::getValue() const
{
    return value;
}

template <typename T>
Primitive<T> Primitive<T>::operator+(const Primitive& other) const
{
    return Primitive(value + other.value);
}

template <typename T>
Primitive<T> Primitive<T>::operator-(const Primitive& other) const
{
    return Primitive(value - other.value);
}

template <typename T>
Primitive<T> Primitive<T>::operator*(const Primitive& other) const
{
    return Primitive(value * other.value);
}

template <typename T>
Primitive<T> Primitive<T>::operator/(const Primitive& other) const
{
    return Primitive(value / other.value);
}

template <typename T>
Primitive<T> Primitive<T>::operator%(const Primitive& other) const
{
    return Primitive(value % other.value);
}

template <typename T>
Primitive<T>& Primitive<T>::operator++()
{
    value++;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator--()
{
    value--;
    return *this;
}

// Post-increment: return old value
template <typename T>
Primitive<T> Primitive<T>::operator++(int)
{
    Primitive<T> old(*this); // copy current value
    ++value;                  // increment this
    return old;               // return copy
}

template <typename T>
Primitive<T> Primitive<T>::operator--(int)
{
    Primitive<T> old(*this); // copy current value
    --value;                  // decrement this
    return old;               // return copy
}

template <typename T>
Primitive<T> Primitive<T>::operator-() const
{
    return Primitive<T>(-value);
}

template <typename T>
Primitive<T>& Primitive<T>::operator+=(const Primitive& other)
{
    value += other.value;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator-=(const Primitive& other)
{
    value -= other.value;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator*=(const Primitive& other)
{
    value *= other.value;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator/=(const Primitive& other)
{
    value /= other.value;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator%=(const Primitive& other)
{
    value %= other.value;
    return *this;
}

template <typename T>
Primitive<T> Primitive<T>::operator+(const T& other) const
{
    return Primitive<T>(value + other);
}

template <typename T>
Primitive<T> Primitive<T>::operator-(const T& other) const
{
    return Primitive<T>(value - other);
}

template <typename T>
Primitive<T> Primitive<T>::operator*(const T& other) const
{
    return Primitive<T>(value * other);
}

template <typename T>
Primitive<T> Primitive<T>::operator/(const T& other) const
{
    return Primitive<T>(value / other);
}

template <typename T>
Primitive<T> Primitive<T>::operator%(const T& other) const
{
    return Primitive<T>(value % other);
}

template <typename T>
Primitive<T>& Primitive<T>::operator+=(const T& other)
{
    value += other;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator-=(const T& other)
{
    value -= other;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator*=(const T& other)
{
    value *= other;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator/=(const T& other)
{
    value /= other;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator%=(const T& other)
{
    value %= other;
    return *this;
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator+(const Primitive<U>& other) const
{
    return Primitive<T>(value + static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator-(const Primitive<U>& other) const
{
    return Primitive<T>(value - static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator*(const Primitive<U>& other) const
{
    return Primitive<T>(value * static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator/(const Primitive<U>& other) const
{
    return Primitive<T>(value / static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator%(const Primitive<U>& other) const
{
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "Modulo only allowed for integral types");
    return Primitive<T>(value % static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator+(const U& other) const
{
    return Primitive<T>(value + static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator-(const U& other) const
{
    return Primitive<T>(value - static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator*(const U& other) const
{
    return Primitive<T>(value * static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator/(const U& other) const
{
    return Primitive<T>(value / static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator%(const U& other) const
{
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "Modulo only allowed for integral types");
    return Primitive<T>(value % static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator+=(const Primitive<U>& other)
{
    value += static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator-=(const Primitive<U>& other)
{
    value -= static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator*=(const Primitive<U>& other)
{
    value *= static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator/=(const Primitive<U>& other)
{
    value /= static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator%=(const Primitive<U>& other)
{
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "Modulo only allowed for integral types");
    value %= static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator+=(const U& other)
{
    value += static_cast<T>(other);
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator-=(const U& other)
{
    value -= static_cast<T>(other);
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator*=(const U& other)
{
    value *= static_cast<T>(other);
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator/=(const U& other)
{
    value /= static_cast<T>(other);
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator%=(const U& other)
{
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "Modulo only allowed for integral types");
    value %= static_cast<T>(other);
    return *this;
}

// Primitive<T> vs Primitive<T>
template <typename T>
Primitive<bool> Primitive<T>::operator>(const Primitive<T>& other) const
{
    return Primitive<bool>(value > other.value);
}

template <typename T>
Primitive<bool> Primitive<T>::operator>=(const Primitive<T>& other) const
{
    return Primitive<bool>(value >= other.value);
}

template <typename T>
Primitive<bool> Primitive<T>::operator<(const Primitive<T>& other) const
{
    return Primitive<bool>(value < other.value);
}

template <typename T>
Primitive<bool> Primitive<T>::operator<=(const Primitive<T>& other) const
{
    return Primitive<bool>(value <= other.value);
}

// Primitive<T> vs T
template <typename T>
Primitive<bool> Primitive<T>::operator>(const T& other) const
{
    return Primitive<bool>(value > other);
}

template <typename T>
Primitive<bool> Primitive<T>::operator>=(const T& other) const
{
    return Primitive<bool>(value >= other);
}

template <typename T>
Primitive<bool> Primitive<T>::operator<(const T& other) const
{
    return Primitive<bool>(value < other);
}

template <typename T>
Primitive<bool> Primitive<T>::operator<=(const T& other) const
{
    return Primitive<bool>(value <= other);
}

// Primitive<T> vs Primitive<U>
template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator>(const Primitive<U>& other) const
{
    return Primitive<bool>(value > other.getValue());
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator>=(const Primitive<U>& other) const
{
    return Primitive<bool>(value >= other.getValue());
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator<(const Primitive<U>& other) const
{
    return Primitive<bool>(value < other.getValue());
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator<=(const Primitive<U>& other) const
{
    return Primitive<bool>(value <= other.getValue());
}

// Primitive<T> vs U
template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator>(const U& other) const
{
    return Primitive<bool>(value > other);
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator>=(const U& other) const
{
    return Primitive<bool>(value >= other);
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator<(const U& other) const
{
    return Primitive<bool>(value < other);
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator<=(const U& other) const
{
    return Primitive<bool>(value <= other);
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator==(const Primitive<U>& other) const
{
    return Primitive<bool>(value == static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator!=(const Primitive<U>& other) const
{
    return Primitive<bool>(value != static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator==(const U& other) const
{
    return Primitive<bool>(value == static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<bool> Primitive<T>::operator!=(const U& other) const
{
    return Primitive<bool>(value != static_cast<T>(other));
}

template <typename T>
Primitive<bool> Primitive<T>::operator||(const Primitive<bool>& other)
{
    return Primitive<bool>(value || other.value);
}

template <typename T>
Primitive<bool> Primitive<T>::operator&&(const Primitive<bool>& other)
{
    return Primitive<bool>(value && other.value);
}

template <typename T>
Primitive<bool> Primitive<T>::operator||(bool other)
{
    return Primitive<bool>(value || other);
}

template <typename T>
Primitive<bool> Primitive<T>::operator&&(bool other)
{
    return Primitive<bool>(value && other);
}

template <typename T>
Primitive<T>::operator bool() const
{
    if constexpr (std::is_same_v<T, bool>) return value;

    return value == T();
}
)" },
    { R"(SafePtr.h)", R"(#pragma once
#include <memory>

#include "Object.h"

// 1. Forward declare Array
template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
class Array;

// 2. Define the detector
template <typename T>
struct is_array_specialization : std::false_type {};

template <typename T>
struct is_array_specialization<Array<T>> : std::true_type {};

// 3. Define the return type trait
template <typename T>
struct get_element_type {
    using type = T;
};

template <typename U>
struct get_element_type<Array<U>> {
    // Array holds SafePtr<Primitive<U>>, so return the Primitive<U>
    using type = typename Array<U>::SafeType;
};

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
class SafePtr : public Object
{
private:
    std::unique_ptr<Object> ptr;
public:
    SafePtr();
    explicit SafePtr(const std::unique_ptr<T>& ptr);
    explicit SafePtr(const T& t);
    SafePtr(const SafePtr& other);

    template <typename U>
    requires (
        std::is_convertible_v<U, T> &&
        (
            std::is_arithmetic_v<U> ||
            std::is_base_of_v<Object, std::remove_cvref_t<U>>
        )
    )
    explicit SafePtr(std::unique_ptr<U>& ptr);

    template <typename U>
    requires (
        std::is_convertible_v<U, T> &&
        (
            std::is_arithmetic_v<U> ||
            std::is_base_of_v<Object, std::remove_cvref_t<U>>
        )
    )
    explicit SafePtr(const U& u);

    template <typename U>
    requires (
        std::is_convertible_v<U, T> &&
        (
            std::is_arithmetic_v<U> ||
            std::is_base_of_v<Object, std::remove_cvref_t<U>>
        )
    )
    explicit SafePtr(const SafePtr<U>& other);

    SafePtr& operator=(const T& t);
    SafePtr& operator=(const SafePtr& t);

    template <typename U>
    requires (
        std::is_convertible_v<U, T> &&
        (
            std::is_arithmetic_v<U> ||
            std::is_base_of_v<Object, std::remove_cvref_t<U>>
        )
    )
    SafePtr& operator=(const U& u);

    template <typename U>
    requires (
        std::is_convertible_v<U, T> &&
        (
            std::is_arithmetic_v<U> ||
            std::is_base_of_v<Object, std::remove_cvref_t<U>>
        )
    )
    SafePtr& operator=(const SafePtr<U>& u);

    SafePtr cloneSafePtr() const;

    get_element_type<T>::type& operator[](int index) override;
    get_element_type<T>::type& operator[](Primitive<int> index) override;

    T* operator->() const;
    T& operator*() const;
    T* get() const;
    std::unique_ptr<Object> clonePtr() const;

    friend std::ostream& operator<<(std::ostream& os, const SafePtr<T>& safePtr)
    {
        os << *(safePtr.ptr);
        return os;
    }

    ~SafePtr() override = default;

    std::string toString() const override;
    std::unique_ptr<Object> clone() const override;

protected:
    Primitive<bool> equals(const Object& other) const override;
};

#include "SafePtr.tpp"
)" },
    { R"(SafePtr.tpp)", R"(#pragma once
#include <algorithm>

#include "Object.h"
#include "SafePtr.h"
#include "String.h"

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::unique_ptr<Object> SafePtr<T>::clonePtr() const
{
    return ptr->clone();
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::string SafePtr<T>::toString() const
{
    return ptr->toString();
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::unique_ptr<Object> SafePtr<T>::clone() const
{
    return ptr->clone();
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Primitive<bool> SafePtr<T>::equals(const Object& other) const
{
    if (this == &other) return Primitive<bool>(true);

    if (auto p = dynamic_cast<const SafePtr<T>*>(&other))
    {
        return *this == *p;
    }

    if (auto p = dynamic_cast<const T*>(&other))
    {
        return *ptr == *p;
    }

    return Primitive<bool>(false);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr()
{

    if constexpr (std::is_same_v<T, Object>)
    {
        ptr = std::make_unique<String>();
    }
    else
    {
        ptr = std::make_unique<T>();
    }
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr(const std::unique_ptr<T>& ptr)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");

    this->ptr = std::unique_ptr<T>(ptr.get());
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr(const T& t)
{
    if constexpr (!std::is_arithmetic_v<T>) static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");

    if constexpr (std::is_same_v<T, Object>)
    {
        ptr = std::move(t.clone());
    }
    else
    {
        ptr = std::make_unique<T>(t);
    }
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr(const SafePtr& other)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");
    ptr = std::move(other.clonePtr());
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (
        std::is_arithmetic_v<U> ||
        std::is_base_of_v<Object, std::remove_cvref_t<U>>
    )
)
SafePtr<T>::SafePtr(std::unique_ptr<U>& ptr)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");
    static_assert(std::is_base_of_v<Object, U>, "non-Object-derived type created");
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");


    this->ptr = std::move(ptr.clone());
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (
        std::is_arithmetic_v<U> ||
        std::is_base_of_v<Object, std::remove_cvref_t<U>>
    )
)
SafePtr<T>::SafePtr(const U& u)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");
    static_assert(std::is_base_of_v<Object, U>, "non-Object-derived type created");
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");

    ptr = std::make_unique<T>(dynamic_cast<T&>(u));
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (
        std::is_arithmetic_v<U> ||
        std::is_base_of_v<Object, std::remove_cvref_t<U>>
    )
)
SafePtr<T>::SafePtr(const SafePtr<U>& other)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");
    static_assert(std::is_base_of_v<Object, U>, "non-Object-derived type created");
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");

    ptr = std::unique_ptr<T>(dynamic_cast<T*>(other.ptr->clone()));
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>& SafePtr<T>::operator=(const T& t)
{
    ptr = std::make_unique<T>(t);
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>& SafePtr<T>::operator=(const SafePtr& t)
{
    ptr = std::move(t.clonePtr());
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (
        std::is_arithmetic_v<U> ||
        std::is_base_of_v<Object, std::remove_cvref_t<U>>
    )
)
SafePtr<T>& SafePtr<T>::operator=(const U& u)
{
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");

    if constexpr (std::is_same_v<T, Object>)
    {
        ptr = u.clone();
    }
    else
    {
        ptr = std::make_unique<T>(dynamic_cast<T>(u));
    }

    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (
        std::is_arithmetic_v<U> ||
        std::is_base_of_v<Object, std::remove_cvref_t<U>>
    )
)
SafePtr<T>& SafePtr<T>::operator=(const SafePtr<U>& u)
{
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");

    ptr = u.clonePtr();
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T> SafePtr<T>::cloneSafePtr() const
{
    return SafePtr<T>(*this);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
typename get_element_type<T>::type& SafePtr<T>::operator[](int index)
{
    T* container = get();

    if constexpr (is_array_specialization<T>::value) {
        // (*container)[index] gives us the SafePtr<U>& stored in the Array.
        // We use '*' to dereference that SafePtr and get the actual Primitive<U>&.
        // This ensures assignment changes the REAL memory inside the array.
        return *((*container)[index]);
    } else {
        using ReturnType = typename get_element_type<T>::type;
        return static_cast<ReturnType&>((*container)[index]);
    }
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
typename get_element_type<T>::type& SafePtr<T>::operator[](Primitive<int> index)
{
    return operator[](index.getValue());
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
T* SafePtr<T>::operator->() const
{
    //if constexpr (!std::is_convertible_v<Object*, T*>) throw std::logic_error("Cannot cast");
    return dynamic_cast<T*>(ptr.get());
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
T& SafePtr<T>::operator*() const
{
    //if constexpr (!std::is_convertible_v<Object&, T&>) throw std::logic_error("Cannot cast");
    return dynamic_cast<T&>(*ptr);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
T* SafePtr<T>::get() const
{
    return reinterpret_cast<T*>(static_cast<void*>(ptr.get()));
})" },
    { R"(String.cpp)", R"(#include "String.h"

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
)" },
    { R"(String.h)", R"(#pragma once
#include "Object.h"
#include "Primitive.h"

class String : public Object
{
private:
    std::string value;

public:
    String();
    explicit String(const std::string& basicString);
    explicit String(const String& string);
    explicit String(const char* basicCharPtr);
    std::string toString() const override;
    std::unique_ptr<Object> clone() const override;
    String& operator=(const String& other);
    String& operator=(const Object& other);
    String& operator=(const char* basicCharPtr);
    String operator+(const char* basicCharPtr) const;
    String& operator+=(const char* basicCharPtr);

    String operator+(const Object& obj) const;
    String& operator+=(const Object& obj);

    template <typename T>
    String& operator=(const Primitive<T>& other)
    {
        value = other.toString();
        return *this;
    }

    template <typename T>
    String operator*(const Primitive<T>& p) const
    {
        std::string str;
        for (int i = 0; i < static_cast<int>(p.getValue()); i++)
        {
            str += value;
        }

        return String(str);
    }

    template <typename T>
    String& operator*=(const Primitive<T>& p)
    {
        *this = *this * p;
        return *this;
    }

    template <typename T>
    String operator*(const T& p) const
    {
        if constexpr (!std::is_arithmetic_v<T>) throw std::logic_error("Cannot cast");
        std::string str;
        for (int i = 0; i < p.getValue(); i++)
        {
            str += value;
        }

        return String(str);
    }

    template <typename T>
    String& operator*=(const T& p)
    {
        if constexpr (!std::is_arithmetic_v<T>) throw std::logic_error("Cannot cast");
        std::string str;
        for (int i = 0; i < static_cast<int>(p); i++)
        {
            str += value;
        }

        value = str;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const String& str);


protected:
    Primitive<bool> equals(const Object& other) const override;
};
)" },
    { R"(Utils.h)", R"(#pragma once
#include "Object.h"

class Utils
{
public:
    template <typename T>
    static T& cast(Object& other);

    template <typename T>
    static T& cast(SafePtr<Object>& other);
};

#include "Utils.tpp")" },
    { R"(Utils.tpp)", R"(#pragma once
#include "Primitive.h"
#include "Utils.h"

template <typename T>
T& Utils::cast(Object& other)
{
    if (auto p = dynamic_cast<T*>(other))
    {
        return *p;
    }

    throw std::logic_error("Illegal cast");
}

template <typename T>
T& Utils::cast(SafePtr<Object>& other)
{
    if (auto o = other.get())
    {
        if (auto p = dynamic_cast<T*>(o))
            return *p;
    }

    throw std::logic_error("Illegal cast");
}
)" },
};

void LibHelper::write(const std::filesystem::path& dir)
{
    std::filesystem::create_directories(dir / "includes");
    for (const auto& [path, content] : files)
    {
        const std::filesystem::path outPath = dir / "includes" / path;
        std::ofstream out(outPath, std::ios::binary);
        out << content;
    }
}
