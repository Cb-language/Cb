#pragma once
#include "Object.h"
#include "SafePtr.h"

class Utils
{
public:

    template <typename T>
    static T& cast(const SafePtr<Object>& other);
};

#include "Utils.tpp"