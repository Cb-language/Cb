#pragma once
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
    SafePtr(const std::unique_ptr<T>& ptr);
    SafePtr(const T& t);
    SafePtr(const SafePtr& other);

    template <typename U> requires std::is_base_of_v<T, U>
    SafePtr(std::unique_ptr<U>& otherPtr);

    template <typename U> requires std::is_base_of_v<T, U>
    SafePtr(const U& u);

    template <typename U>
    SafePtr(const SafePtr<U>& other);

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
    const T* get() const override;
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

#include "SafePtr.tpp"