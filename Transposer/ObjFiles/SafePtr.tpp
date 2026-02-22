#pragma once
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
template <typename U>
SafePtr<T>::SafePtr(std::unique_ptr<U>& otherPtr)
{
    ptr = otherPtr.release();
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
SafePtr<T>::SafePtr(const U& u)
{
    ptr = std::make_unique<T>(u);
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

// Template Constructor (UniquePtr)
template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (std::is_arithmetic_v<U> || std::is_base_of_v<Object, std::remove_cvref_t<U>>)
)
SafePtr<T>::SafePtr(std::unique_ptr<U>& otherPtr)
{
    this->ptr = std::move(otherPtr);
}

// Template Constructor (Value)
template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (std::is_arithmetic_v<U> || std::is_base_of_v<Object, std::remove_cvref_t<U>>)
)
SafePtr<T>::SafePtr(const U& u)
{
    // Use SafeType to ensure we wrap in Primitive if U is arithmetic
    using InternalType = typename Array<T>::SafeType;
    ptr = std::make_unique<InternalType>(u);
}

// Template Constructor (Cross-type SafePtr)
template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (std::is_arithmetic_v<U> || std::is_base_of_v<Object, std::remove_cvref_t<U>>)
)
SafePtr<T>::SafePtr(const SafePtr<U>& other)
{
    ptr = other.clonePtr();
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

// Template Assignment (Value)
template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (std::is_arithmetic_v<U> || std::is_base_of_v<Object, std::remove_cvref_t<U>>)
)
SafePtr<T>& SafePtr<T>::operator=(const U& u)
{
    if constexpr (std::is_same_v<T, Object>)
        ptr = u.clone();
    else
        ptr = std::make_unique<T>(static_cast<const T&>(u));
    return *this;
}

// Template Assignment (Cross-type SafePtr)
template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
requires (
    std::is_convertible_v<U, T> &&
    (std::is_arithmetic_v<U> || std::is_base_of_v<Object, std::remove_cvref_t<U>>)
)
SafePtr<T>& SafePtr<T>::operator=(const SafePtr<U>& u)
{
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
    T& container = *(*this);
    return container[index];
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
typename get_element_type<T>::type& SafePtr<T>::operator[](Primitive<int> index)
{
    return this->operator[](index.getValue());
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
}