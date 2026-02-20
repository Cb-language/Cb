#pragma once

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
