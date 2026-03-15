#pragma once
#include "Primitive.h"
#include "Utils.h"

template <typename T, typename U>
T Utils::cast(const U& other)
{
    if (auto o = other.get())
    {
        if (auto p = dynamic_cast<typename T::InnerT*>(o))
            return SafePtr(*p);
    }

    throw std::logic_error(badCastMsg);
}

template <typename T, typename U>
bool Utils::is(const U& other)
{
    try
    {
        cast<T, U>(other);
    }
    catch (std::logic_error& e)
    {
        if (e.what() != badCastMsg) throw e;
        return false;
    }

    return true;
}
