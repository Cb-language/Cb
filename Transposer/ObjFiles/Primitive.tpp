#pragma once
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
std::string Primitive<T>::toString() const {
    if constexpr (std::is_same_v<T, bool>) return value ? "cres" : "demen";
    return std::to_string(value);
}

template <typename T>
requires std::is_arithmetic_v<T>
std::unique_ptr<Object> Primitive<T>::clone() const {
    return std::make_unique<Primitive>(value);
}

template <typename T>
requires std::is_arithmetic_v<T>
T Primitive<T>::getValue() const { return value; }

// Math Implementation
template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T> Primitive<T>::operator+(const Primitive& other) const { return Primitive(value + other.value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T> Primitive<T>::operator-(const Primitive& other) const { return Primitive(value - other.value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T> Primitive<T>::operator*(const Primitive& other) const { return Primitive(value * other.value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T> Primitive<T>::operator/(const Primitive& other) const { return Primitive(value / other.value); }

template <typename T>
requires std::is_arithmetic_v<T>
Primitive<T> Primitive<T>::operator%(const Primitive& other) const {
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
    value -= other.value;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator*=(const U& other)
{
    value *= other.value;
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<T>& Primitive<T>::operator/=(const U& other)
{
    value /= other.value;
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
    return Primitive(value >= other.value);
}

template <typename T> requires std::is_arithmetic_v<T>
template <typename U>
Primitive<bool> Primitive<T>::operator<=(const U& other) const
{
    return Primitive(value <= other.value);
}

template <typename T> requires std::is_arithmetic_v<T>
Primitive<bool> Primitive<T>::operator||(bool other)
{
    return Primitive(value || other);
}

template <typename T> requires std::is_arithmetic_v<T>
Primitive<bool> Primitive<T>::operator&&(bool other)
{
    return Primitive(value && other);
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
