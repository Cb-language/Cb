#pragma once
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

#include "Primitive.tpp"