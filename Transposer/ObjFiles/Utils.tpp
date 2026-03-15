#pragma once
#include "Primitive.h"
#include "Utils.h"

template <typename T>
SafePtr<T> Utils::cast(const SafePtr<Object>& other)
{
    if (auto o = other.get())
    {
        if (auto p = dynamic_cast<T*>(o))
            return SafePtr<T>(*p);
    }

    throw std::logic_error(badCastMsg);
}

template <typename T>
bool Utils::is(const SafePtr<Object>& other)
{
    try
    {
        cast<T>(other);
    }
    catch (std::logic_error& e)
    {
        if (e.what() != badCastMsg) throw e;
        return false;
    }

    return true;
}
