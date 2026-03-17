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

    std::string toString(int indents = 0) const override;

protected:
    Primitive<bool> equals(const Object& other) const override;

private:
    Primitive<unsigned int> size = Primitive<unsigned int>(0);
    SafePtr<T> defaultValueSet = SafePtr<T>();
    std::vector<SafePtr<T>> data;

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
    defaultValueSet = SafePtr<T>();
    data.push_back(defaultValueSet.cloneSafePtr());
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(Primitive<unsigned int> size) : Array(size, SafePtr<T>())
{
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(Primitive<unsigned int> size, SafePtr<T> defaultValue)
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
Array<T>::Array(Primitive<unsigned int> size, T defaultValue) : Array<T>(size, SafePtr<T>(T(defaultValue)))
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
        data.push_back(SafePtr<T>(other.data[i.getValue()].cloneSafePtr()));
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
        defaultValueSet = SafePtr<T>();
        for (Primitive<int> i; i < size; ++i)
        {
            data.push_back(SafePtr<T>(static_cast<T>(*other[i])));
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
        data.push_back(other.data[i.getValue()].cloneSafePtr());
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
            data.push_back(SafePtr<T>(static_cast<T>(*other[i])));
        }
    }

    defaultValueSet = data[0].cloneSafePtrPtr();
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>& Array<T>::operator=(const T& defaultValue)
{
    data.clear();
    data.reserve(size);

    defaultValueSet = SafePtr<T>(defaultValue);
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
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
        defaultValueSet = SafePtr<T>(static_cast<T>(defaultValue));
    }

    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
    }

    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>& Array<T>::operator[](Primitive<int> index)
{
    index = getNormalIndex(index);
    return data[index.getValue()];
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
const SafePtr<T>& Array<T>::operator[](Primitive<int> index) const
{
    index = getNormalIndex(index);
    return data[index.getValue()];
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>& Array<T>::operator[](int index)
{
    int i = getNormalIndex(index).getValue();
    return data[i];
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
const SafePtr<T>& Array<T>::operator[](int index) const
{
    index = getNormalIndex(index);
    return data[index];
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
std::enable_if_t<std::is_base_of_v<Object, U>, void> Array<T>::add(const U& obj)
{
    data.push_back(SafePtr<T>(obj.cloneSafePtr()));
    ++size;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
void Array<T>::add(T& value, Primitive<int> index)
{
    if (index == Primitive<int> (-1) || index >= size)
        index = size;

    data.insert(data.begin() + index, SafePtr<T>(value));
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
        result.data.push_back(data[i.getValue()].cloneSafePtr());
        ++result.size;
    }

    result.defaultValueSet = defaultValueSet.cloneSafePtr();
    return result;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::string Array<T>::toString(int indents) const
{
    std::string str = getIndents(indents) + "[";
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
)" },
    { R"(Object.h)", R"(#pragma once
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

protected:
    virtual Primitive<bool> equals(const Object& other) const;
    static std::string getIndents(int indents);
};
)" },
    { R"(Primitive.h)", R"(#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <type_traits>

class Object;

class PrimitiveBase : public Object
{
public:
    virtual ~PrimitiveBase() = default;
    virtual double getNumericValue() const = 0;
};

// Helper to prevent Primitive<Primitive<T>>
template <typename T>
struct is_primitive : std::false_type {};

template <typename T>
requires std::is_arithmetic_v<T>
class Primitive;

template <typename T>
struct is_primitive<Primitive<T>> : std::true_type {};

template <typename T>
requires std::is_arithmetic_v<T>
class Primitive : public PrimitiveBase
{
private:
    T value;

public:
    double getNumericValue() const override;
    explicit Primitive(const T& value = T());
    Primitive(const Primitive& other);

    template <typename U>
    Primitive(const Primitive<U>& other);

    Primitive& operator=(const Primitive& other);
    Primitive& operator=(const T& other);

    template <typename U>
    Primitive& operator=(const Primitive<U>& other);

    template <typename U>
    Primitive& operator=(const U& other);

    Primitive<T>& operator=(const Object& other);

    std::string toString(int indents = 0) const override;
    T getValue() const;

    friend std::istream& operator>>(std::istream& is, Primitive<T>& obj)
    {
        is >> obj.value;
        return is;
    }

    // Basic Math
    Primitive<double> operator+(const Primitive& other) const;
    Primitive<double> operator-(const Primitive& other) const;
    Primitive<double> operator*(const Primitive& other) const;
    Primitive<double> operator/(const Primitive& other) const;
    Primitive<int> operator%(const Primitive& other) const;

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

    // Template Math vs Raw Types
    template <typename U> Primitive operator+(const U& other) const;
    template <typename U> Primitive operator-(const U& other) const;
    template <typename U> Primitive operator*(const U& other) const;
    template <typename U> Primitive operator/(const U& other) const;
    template <typename U> Primitive operator%(const U& other) const;

    template <typename U> Primitive& operator+=(const U& other);
    template <typename U> Primitive& operator-=(const U& other);
    template <typename U> Primitive& operator*=(const U& other);
    template <typename U> Primitive& operator/=(const U& other);
    template <typename U> Primitive& operator%=(const U& other);

    // Comparison
    template <typename U> Primitive<bool> operator>(const U& other) const;
    template <typename U> Primitive<bool> operator>=(const U& other) const;
    template <typename U> Primitive<bool> operator<(const U& other) const;
    template <typename U> Primitive<bool> operator<=(const U& other) const;
    template <typename U> Primitive<bool> operator==(const U& other) const;
    template <typename U> Primitive<bool> operator!=(const U& other) const;

    // Logic
    Primitive<bool> operator||(const Primitive<bool>& other);
    Primitive<bool> operator&&(const Primitive<bool>& other);
    Primitive<bool> operator||(bool other);
    Primitive<bool> operator&&(bool other);
    explicit operator bool() const;

protected:
    Primitive<bool> equals(const Object& other) const override;
};

// GLOBAL OPERATORS - CRITICAL FOR AMBIGUITY FIX
// These use 'requires' to ensure they only work when the Left Hand Side is NOT a Primitive
template <typename T, typename U>
requires std::is_arithmetic_v<U> && (!is_primitive<U>::value)
auto operator+(U left, const Primitive<T>& right) { return Primitive<T>(left + right.getValue()); }

template <typename T, typename U>
requires std::is_arithmetic_v<U> && (!is_primitive<U>::value)
auto operator-(U left, const Primitive<T>& right) { return Primitive<T>(left - right.getValue()); }

template <typename T, typename U>
requires std::is_arithmetic_v<U> && (!is_primitive<U>::value)
auto operator*(U left, const Primitive<T>& right) { return Primitive<T>(left * right.getValue()); }

template <typename T, typename U>
requires std::is_arithmetic_v<U> && (!is_primitive<U>::value)
auto operator/(U left, const Primitive<T>& right) { return Primitive<T>(left / right.getValue()); }

#include "Primitive.tpp")" },
    { R"(Primitive.tpp)", R"(#pragma once
#include <iomanip>

#include "Primitive.h"


template <typename T>
requires std::is_arithmetic_v<T>
double Primitive<T>::getNumericValue() const { return static_cast<double>(value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T>::Primitive(const T& value) : value(value) {}

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T>::Primitive(const Primitive& other) : value(other.value) {}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>::Primitive(const Primitive<U>& other) : value(other.getValue())
{
}

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator=(const Primitive& other) {
    if (this != &other) value = other.value;
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator=(const T& other) {
    value = other;
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator=(const Primitive<U>& other) {
    value = static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator=(const U& other) {
    value = static_cast<T>(other);
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator=(const Object& other) {
    if (this == &other) return *this;
    if (const auto* p = dynamic_cast<const Primitive<T>*>(&other)) {
        value = p->value;
    } else if (const auto* pb = dynamic_cast<const PrimitiveBase*>(&other)) {
        value = static_cast<T>(pb->getNumericValue());
    } else {
        throw std::logic_error("Cannot cast Object to Primitive");
    }
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<bool> Primitive<T>::equals(const Object& other) const {
    if (const auto* pBase = dynamic_cast<const PrimitiveBase*>(&other)) {
        return Primitive<bool>(getNumericValue() == pBase->getNumericValue());
    }
    return Primitive<bool>(false);
}

template <typename T>
requires std::is_arithmetic_v<T>
std::string Primitive<T>::toString(int indents) const {
    if constexpr (std::is_same_v<T, bool>) return getIndents(indents) + (value ? "cres" : "demen");
    if constexpr (std::is_same_v<T, double>)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << value;
        std::string s = oss.str();

        // remove trailing zeros, but leave at least one decimal
        size_t dotPos = s.find('.');
        if (dotPos != std::string::npos) {
            // remove trailing zeros after the dot
            size_t lastNonZero = s.find_last_not_of('0');
            if (lastNonZero == dotPos) {
                // if all decimals were zeros, leave one zero
                lastNonZero++;
            }
            s.erase(lastNonZero + 1);
        }

        return getIndents(indents) + s;
    }
    return getIndents(indents) + std::to_string(value);
}

template <typename T>
requires std::is_arithmetic_v<T>
T Primitive<T>::getValue() const { return value; }

// Math Implementation
template <typename T>
requires std::is_arithmetic_v<T>
Primitive<double> Primitive<T>::operator+(const Primitive& other) const { return Primitive(value + other.value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<double> Primitive<T>::operator-(const Primitive& other) const { return Primitive(value - other.value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<double> Primitive<T>::operator*(const Primitive& other) const { return Primitive(value * other.value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<double> Primitive<T>::operator/(const Primitive& other) const { return Primitive(value / other.value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<int> Primitive<T>::operator%(const Primitive& other) const {
    if constexpr (std::is_integral_v<T>) return Primitive(value % other.value);
    return Primitive(T());
}

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T> Primitive<T>::operator-() const { return Primitive<T>(-value); }

template <typename T> requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator+=(const Primitive& other)
{
    value += other.value;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator-=(const Primitive& other)
{
    value -= other.value;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator*=(const Primitive& other)
{
    value *= other.value;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator/=(const Primitive& other)
{
    value /= other.value;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator%=(const Primitive& other)
{
    value %= other.value;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T> Primitive<T>::operator%(const U& other) const
{
    return Primitive(value % other);
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator-=(const U& other)
{
    value -= other;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator*=(const U& other)
{
    value *= other;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator/=(const U& other)
{
    value /= other;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator%=(const U& other)
{
    value %= other.value;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<bool> Primitive<T>::operator>=(const U& other) const
{
    return Primitive<bool>(value >= other);
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<bool> Primitive<T>::operator<=(const U& other) const
{
    return Primitive<bool>(value <= other);
}

template <typename T> requires std::is_arithmetic_v<T>
Primitive<bool> Primitive<T>::operator||(bool other)
{
    return Primitive<bool>(value || other);
}

template <typename T> requires std::is_arithmetic_v<T>
Primitive<bool> Primitive<T>::operator&&(bool other)
{
    return Primitive<bool>(value && other);
}

// Generic Template Math (Handles both T and Primitive<U>)
template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T> Primitive<T>::operator+(const U& other) const {
    if constexpr (is_primitive<U>::value) return Primitive<T>(value + static_cast<T>(other.getValue()));
    else return Primitive<T>(value + static_cast<T>(other));
}

template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T> Primitive<T>::operator-(const U& other) const {
    if constexpr (is_primitive<U>::value) return Primitive<T>(value - static_cast<T>(other.getValue()));
    else return Primitive<T>(value - static_cast<T>(other));
}

template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T> Primitive<T>::operator*(const U& other) const {
    if constexpr (is_primitive<U>::value) return Primitive<T>(value * static_cast<T>(other.getValue()));
    else return Primitive<T>(value * static_cast<T>(other));
}

template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T> Primitive<T>::operator/(const U& other) const {
    if constexpr (is_primitive<U>::value) return Primitive<T>(value / static_cast<T>(other.getValue()));
    else return Primitive<T>(value / static_cast<T>(other));
}

// Increment/Decrement
template <typename T> requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator++() { ++value; return *this; }
template <typename T> requires std::is_arithmetic_v<T>
Primitive<T>& Primitive<T>::operator--() { --value; return *this; }
template <typename T> requires std::is_arithmetic_v<T>
Primitive<T> Primitive<T>::operator++(int) { Primitive tmp(*this); ++value; return tmp; }
template <typename T> requires std::is_arithmetic_v<T>
Primitive<T> Primitive<T>::operator--(int) { Primitive tmp(*this); --value; return tmp; }

// Compound assignments
template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator+=(const U& other) {
    if constexpr (is_primitive<U>::value) value += static_cast<T>(other.getValue());
    else value += static_cast<T>(other);
    return *this;
}

// Comparison
template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<bool> Primitive<T>::operator>(const U& other) const {
    if constexpr (is_primitive<U>::value) return Primitive<bool>(value > other.getValue());
    else return Primitive<bool>(value > other);
}

template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<bool> Primitive<T>::operator<(const U& other) const {
    if constexpr (is_primitive<U>::value) return Primitive<bool>(value < other.getValue());
    else return Primitive<bool>(value < other);
}

template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<bool> Primitive<T>::operator==(const U& other) const {
    if constexpr (is_primitive<U>::value) return Primitive<bool>(value == other.getValue());
    else return Primitive<bool>(value == other);
}

template <typename T>
requires std::is_arithmetic_v<T>
template <typename U>
Primitive<bool> Primitive<T>::operator!=(const U& other) const {
    return Primitive<bool>(!(*this == other).getValue());
}

// Logic Implementation
template <typename T>
requires std::is_arithmetic_v<T>
Primitive<bool> Primitive<T>::operator||(const Primitive<bool>& other) { return Primitive<bool>(static_cast<bool>(*this) || other.getValue()); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<bool> Primitive<T>::operator&&(const Primitive<bool>& other) { return Primitive<bool>(static_cast<bool>(*this) && other.getValue()); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T>::operator bool() const {
    if constexpr (std::is_same_v<T, bool>) return value;
    return value != T(0);
}
)" },
    { R"(SafePtr.h)", R"(#pragma once
#include <memory>
#include "Object.h"

// Forward declarations and traits...
template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
class Array;

template <typename T> struct is_array_specialization : std::false_type {using inner_type = void;};
template <typename T> struct is_array_specialization<Array<T>> : std::true_type {using inner_type = T;};

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
class SafePtr : public Object
{
private:
    std::unique_ptr<T> ptr;
public:
    using InnerT = typename std::conditional_t<
        is_array_specialization<T>::value,
        is_array_specialization<T>,
        is_array_specialization<Array<T>> // Dummy fallback that won't trigger SafePtr<void>
    >::inner_type;
    using InnerSafePtr = SafePtr<InnerT>;
    SafePtr();
    explicit SafePtr(const std::unique_ptr<T>& ptr);
    explicit SafePtr(const T& t);
    SafePtr(const SafePtr& other);

    template <typename U> requires std::is_base_of_v<T, U>
    explicit SafePtr(std::unique_ptr<U>& otherPtr);

    template <typename U> requires std::is_base_of_v<T, U>
    explicit SafePtr(const U& u);

    template <typename U>
    explicit SafePtr(const SafePtr<U>& other);

    template <typename U>
    SafePtr& operator=(const U& u);

    template <typename U>
    SafePtr& operator=(const SafePtr<U>& u);

    // Existing Template Constructors and Assignment Operators...
    // (Omitted for brevity, keep your existing implementations)

    SafePtr& operator=(const T& t);
    SafePtr& operator=(const SafePtr& t);

    // --- Math & Assignment Operators ---

    // Compound Assignments
    template <typename U> SafePtr& operator+=(const U& other) { **ptr += other; return *this; }
    template <typename U> SafePtr& operator-=(const U& other) { **ptr -= other; return *this; }
    template <typename U> SafePtr& operator*=(const U& other) { **ptr *= other; return *this; }
    template <typename U> SafePtr& operator/=(const U& other) { **ptr /= other; return *this; }
    template <typename U> SafePtr& operator%=(const U& other) { **ptr %= other; return *this; }

    // Increment / Decrement
    SafePtr& operator++() { ++(static_cast<T&>(*ptr)); return *this; }
    SafePtr operator++(int) { SafePtr tmp(*this); ++(static_cast<T&>(*ptr)); return tmp; }
    SafePtr& operator--() { --(static_cast<T&>(*ptr)); return *this; }
    SafePtr operator--(int) { SafePtr tmp(*this); --(static_cast<T&>(*ptr)); return tmp; }

    // --- Existing Accessors ---
    SafePtr cloneSafePtr() const;

    T* operator->() const;
    T& operator*() const;
    T* get() const;
    std::unique_ptr<T> clonePtr() const;

    InnerSafePtr& operator[](int index)
    requires is_array_specialization<T>::value;

    InnerSafePtr& operator[](Primitive<int> index)
    requires is_array_specialization<T>::value;

    friend std::ostream& operator<<(std::ostream& os, const SafePtr<T>& safePtr) {
        os << *(safePtr.ptr);
        return os;
    }

    ~SafePtr() override = default;
    std::string toString(int indents = 0) const override;

protected:
    Primitive<bool> equals(const Object& other) const override;
};

// --- Global Binary Operators (The "Bridge") ---
// These allow expressions like: ptr + ptr, ptr + 5, or 5 + ptr

#define SAFE_PTR_BIN_OP(op) \
template <typename T, typename U> \
auto operator op(const SafePtr<T>& lhs, const SafePtr<U>& rhs) { return (*lhs) op (*rhs); } \
template <typename T, typename U> \
auto operator op(const SafePtr<T>& lhs, const U& rhs) { return (*lhs) op rhs; } \
template <typename T, typename U> \
auto operator op(const U& lhs, const SafePtr<T>& rhs) { return lhs op (*rhs); }

SAFE_PTR_BIN_OP(+)
SAFE_PTR_BIN_OP(-)
SAFE_PTR_BIN_OP(*)
SAFE_PTR_BIN_OP(/)
SAFE_PTR_BIN_OP(%)
SAFE_PTR_BIN_OP(==)
SAFE_PTR_BIN_OP(!=)
SAFE_PTR_BIN_OP(<)
SAFE_PTR_BIN_OP(>)
SAFE_PTR_BIN_OP(<=)
SAFE_PTR_BIN_OP(>=)
SAFE_PTR_BIN_OP(&&)
SAFE_PTR_BIN_OP(||)

#undef SAFE_PTR_BIN_OP

#include "SafePtr.tpp")" },
    { R"(SafePtr.tpp)", R"(#pragma once
#include <algorithm>
#include "Object.h"
#include "SafePtr.h"
#include "String.h"

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::unique_ptr<T> SafePtr<T>::clonePtr() const
{
    return std::make_unique<T>(*ptr);
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::InnerSafePtr& SafePtr<T>::operator[](int index) requires is_array_specialization<T>::value
{
    return ptr->operator[](index);
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::InnerSafePtr& SafePtr<T>::operator[](Primitive<int> index) requires is_array_specialization<T>::value
{
    return ptr->operator[](index);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::string SafePtr<T>::toString(int indents) const
{
    return ptr->toString(indents);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Primitive<bool> SafePtr<T>::equals(const Object& other) const
{
    if (this == &other) return Primitive<bool>(true);
    if (auto p = dynamic_cast<const SafePtr<T>*>(&other))
    {
        return *(*this) == *(*p);
    }
    return Primitive<bool>(false);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr()
{
    if constexpr (std::is_same_v<T, Object>)
        ptr = std::make_unique<String>();
    else
        ptr = std::make_unique<T>();
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr(const std::unique_ptr<T>& otherPtr)
{
    this->ptr = otherPtr->clone();
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr(const T& t)
{
    if constexpr (std::is_same_v<T, Object>)
        ptr = t.clone();
    else
        ptr = std::make_unique<T>(t);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr(const SafePtr& other)
{
    ptr = other.clonePtr();
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U> requires std::is_base_of_v<T, U>
SafePtr<T>::SafePtr(std::unique_ptr<U>& otherPtr)
{
    ptr = otherPtr.release();
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U> requires std::is_base_of_v<T, U>
SafePtr<T>::SafePtr(const U& u)
{
    ptr = std::make_unique<U>(u);
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
SafePtr<T>::SafePtr(const SafePtr<U>& other)
{
    ptr = other.clonePtr();
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
SafePtr<T>& SafePtr<T>::operator=(const U& u)
{
    ptr = std::make_unique<T>(u);
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
SafePtr<T>& SafePtr<T>::operator=(const SafePtr<U>& u)
{
    ptr = u.clonePtr();
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>& SafePtr<T>::operator=(const T& t)
{
    if constexpr (std::is_same_v<T, Object>)
        ptr = t.clone();
    else
        ptr = std::make_unique<T>(t);
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>& SafePtr<T>::operator=(const SafePtr& t)
{
    if (this != &t) ptr = t.clonePtr();
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
T* SafePtr<T>::operator->() const
{
    return static_cast<T*>(ptr.get());
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
T& SafePtr<T>::operator*() const
{
    return static_cast<T&>(*ptr);
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
T* SafePtr<T>::get() const
{
    return static_cast<T*>(ptr.get());
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

std::string String::toString(int indents) const
{
    return  getIndents(indents) + value;
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
    std::string toString(int indents = 0) const override;
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

    friend std::ostream& operator<<(std::ostream& os, const String& str)
    {
        os << str.value;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, String& str)
    {
        is >> str.value;
        return is;
    }


protected:
    Primitive<bool> equals(const Object& other) const override;
};
)" },
    { R"(Utils.h)", R"(#pragma once
#include "Object.h"
#include "SafePtr.h"

class Utils
{
public:
    inline static const std::string badCastMsg = "Illegal cast";
    template <typename T, typename U>
    static T cast(const U& other);

    template <typename T, typename U>
    static bool is(const U& other);
};

#include "Utils.tpp")" },
    { R"(Utils.tpp)", R"(#pragma once
#include "Primitive.h"
#include "Utils.h"

template <typename T, typename U>
T Utils::cast(const U& other)
{
    if (auto o = other.get())
    {
        if (auto p = dynamic_cast<typename T::InnerT*>(o))
            return SafePtr(*p);
    }

    throw std::logic_error(badCastMsg);
}

template <typename T, typename U>
bool Utils::is(const U& other)
{
    try
    {
        cast<T, U>(other);
    }
    catch (std::logic_error& e)
    {
        if (e.what() != badCastMsg) throw e;
        return false;
    }

    return true;
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
