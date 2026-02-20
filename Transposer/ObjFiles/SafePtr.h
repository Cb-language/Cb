#pragma once
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
