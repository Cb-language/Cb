#pragma once
#include "Object.h"
#include "SafePtr.h"

class Utils
{
public:
    inline static const std::string badCastMsg = "Illegal cast";
    template <typename T, typename U>
    static T cast(const U& other);

    template <typename T, typename U>
    static bool is(const U& other);
};

#include "Utils.tpp"