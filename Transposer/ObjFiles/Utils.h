#pragma once
#include "Object.h"
#include "SafePtr.h"

class Utils
{
public:
    static const std::string badCastMsg;
    template <typename T>
    static T& cast(const SafePtr<Object>& other);

    template <typename T>
    static bool is(const SafePtr<Object>& other);
};

#include "Utils.tpp"