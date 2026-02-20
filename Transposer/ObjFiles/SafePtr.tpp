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
}