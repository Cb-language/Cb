#pragma once
#include "Primitive.h"
#include "Utils.h"


template <typename T>
T& Utils::cast(const SafePtr<Object>& other)
{
    if (auto o = other.get())
    {
        if (auto p = dynamic_cast<T*>(o))
            return *p;
    }

    throw std::logic_error("Illegal cast");
}
