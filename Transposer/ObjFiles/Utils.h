#pragma once
#include "Object.h"
#include "SafePtr.h"

class Utils
{
public:
    inline static const std::string badCastMsg = "Illegal cast";
    template <typename T>
    static SafePtr<T> cast(const SafePtr<Object>& other);

    template <typename T>
    static bool is(const SafePtr<Object>& other);
};

#include "Utils.tpp"