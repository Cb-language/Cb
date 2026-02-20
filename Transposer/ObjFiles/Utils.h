#pragma once
#include "Object.h"

class Utils
{
public:
    template <typename T>
    static T& cast(Object& other);

    template <typename T>
    static T& cast(SafePtr<Object>& other);
};

#include "Utils.tpp"