#pragma once
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
