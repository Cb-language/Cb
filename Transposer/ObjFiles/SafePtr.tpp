#pragma once
#include <algorithm>
#include "Object.h"
#include "SafePtr.h"
#include "String.h"

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::unique_ptr<T> SafePtr<T>::clonePtr() const
{
    if (ptr == nullptr) return nullptr;
    return std::unique_ptr<T>(const_cast<T*>(this->ptr->clone()));
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
    else if constexpr(std::is_abstract_v<T>)
        ptr = nullptr;
    else
        ptr = std::make_unique<T>();
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>::SafePtr(const std::unique_ptr<T>& ptr)
{
    this->ptr = ptr->clonePtr();
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
    ptr = std::make_unique<U>(u);
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
SafePtr<T>& SafePtr<T>::operator=(const SafePtr<U>& u)
{
    ptr = u.clonePtr();
    return *this;
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
SafePtr<T>& SafePtr<T>::operator=(const Primitive<U>& other)
{
    ptr = std::make_unique<T>(other);
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>& SafePtr<T>::operator=(const T& other)
{
    if constexpr (std::is_same_v<T, Object>)
        ptr = other.clone();
    else
        ptr = std::make_unique<T>(other);
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T>& SafePtr<T>::operator=(const SafePtr& other)
{
    if (this != &other) ptr = other.clonePtr();
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<T> SafePtr<T>::cloneSafePtr() const
{
    return SafePtr<T>(*this);
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
const SafePtr<T>* SafePtr<T>::clone() const
{
    return new SafePtr(*this);
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
const T* SafePtr<T>::get() const
{
    return ptr.get();
}
