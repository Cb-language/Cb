#pragma once
#include "Array.h"
#include "Object.h"
#include "String.h"

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array()
{
    size = 1;
    defaultValueSet = SafePtr<T>();
    data.push_back(defaultValueSet.cloneSafePtr());
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(Primitive<unsigned int> size) : Array(size, SafePtr<T>())
{
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(Primitive<unsigned int> size, SafePtr<UnderlyingT> defaultValue)
    : size(size)
{
    defaultValueSet = defaultValue;

    data.reserve(size.getValue());
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
    }
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(Primitive<unsigned int> size, UnderlyingT defaultValue) : Array<T>(size, SafePtr<T>(T(defaultValue)))
{
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>::Array(const Array<T>& other)
{
    size = other.size;
    defaultValueSet = other.defaultValueSet;
    data.reserve(size.getValue());
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(SafePtr<T>(other.data[i.getValue()].cloneSafePtr()));
    }
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
Array<T>::Array(const Array<U>& other, bool isValArray)
{
    size = other.Length();
    data.reserve(size);

    if constexpr (std::is_same_v<T, std::string>)
    {
        std::ostringstream oss;
        oss << other[0]; // just for default
        defaultValueSet = SafePtr<String>(String(oss.str()));

        for (Primitive<int> i; i < size; ++i)
        {
            std::ostringstream ossData;
            ossData << other[i];
            data.push_back(SafePtr<String>(String(ossData.str())));
        }
    }
    else
    {
        defaultValueSet = SafePtr<T>();
        for (Primitive<int> i; i < size; ++i)
        {
            data.push_back(SafePtr<T>(static_cast<T>(*other[i])));
        }
    }
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>& Array<T>::operator=(const Array<T>& other)
{
    if (this == &other)
        return *this;

    size = other.size;
    defaultValueSet = other.defaultValueSet;

    data.clear();
    data.reserve(size);
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(other.data[i.getValue()].cloneSafePtr());
    }
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
Array<T>& Array<T>::operator=(const Array<U>& other)
{
    size = other.Length();
    data.clear();
    data.reserve(size);

    for (Primitive<int> i; i < size; ++i)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            std::ostringstream oss;
            oss << *other[i];
            data.push_back(SafePtr<String>(String(oss.str())));
        }
        else
        {
            data.push_back(SafePtr<UnderlyingT>(static_cast<UnderlyingT>(*other[i])));
        }
    }

    defaultValueSet = data[0].cloneSafePtrPtr();
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T>& Array<T>::operator=(const UnderlyingT& defaultValue)
{
    data.clear();
    data.reserve(size);

    defaultValueSet = SafePtr<UnderlyingT>(defaultValue);
    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
    }
    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
Array<T>& Array<T>::operator=(const U& defaultValue)
{
    data.clear();
    data.reserve(size);

    if constexpr (std::is_same_v<T, std::string>)
    {
        std::ostringstream oss;
        oss << defaultValue;
        data.push_back(SafePtr<String>(String(oss.str())));
    }
    else
    {
        defaultValueSet = SafePtr<UnderlyingT>(static_cast<UnderlyingT>(defaultValue));
    }

    for (Primitive<int> i; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
    }

    return *this;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<typename Array<T>::UnderlyingT>& Array<T>::operator[](Primitive<int> index)
{
    index = getNormalIndex(index);
    return data[index.getValue()];
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
const SafePtr<typename Array<T>::UnderlyingT>& Array<T>::operator[](Primitive<int> index) const
{
    index = getNormalIndex(index);
    return data[index.getValue()];
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
SafePtr<typename Array<T>::UnderlyingT>& Array<T>::operator[](int index)
{
    int i = getNormalIndex(index).getValue();
    return data[i];
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
const SafePtr<typename Array<T>::UnderlyingT>& Array<T>::operator[](int index) const
{
    index = getNormalIndex(index);
    return data[index];
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
template <typename U>
std::enable_if_t<std::is_base_of_v<Object, U>, void> Array<T>::add(const U& obj)
{
    data.push_back(SafePtr<T>(obj.cloneSafePtr()));
    ++size;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
void Array<T>::add(UnderlyingT& value, Primitive<int> index)
{
    if (index == Primitive<int> (-1) || index >= size)
        index = size;

    data.insert(data.begin() + index, SafePtr<T>(value));
    ++size;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
void Array<T>::remove(Primitive<int> index)
{
    index = getNormalIndex(index);
    data.erase(data.begin() + index.getValue());
    --size;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Array<T> Array<T>::slice(Primitive<int> start, Primitive<int> stop, Primitive<int> step) const
{
    if (step == Primitive<int>(0))
        throw std::out_of_range("Step cannot be zero");

    int sz = static_cast<int>(size.getValue());
    if (start < 0) start += sz;
    if (stop < 0) stop += sz;
    if (stop == Primitive<int>(-1)) stop = sz;

    Array<T> result;
    result.data.clear();

    for (Primitive<int> i = start; (step > 0 ? i < stop : i > stop); i += step)
    {
        result.data.push_back(data[i.getValue()].cloneSafePtr());
        ++result.size;
    }

    result.defaultValueSet = defaultValueSet.cloneSafePtr();
    return result;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
std::string Array<T>::toString(int indents) const
{
    std::string str = getIndents(indents) + "[";
    bool first = true;

    for (Primitive<int> i; i < size; ++i)
    {
        if (!first) str += ", ";
        str += data[i.getValue()]->toString();
        first = false;
    }

    str += "]";
    return str;
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Primitive<bool> Array<T>::equals(const Object& other) const
{
    if (this == &other) return Primitive<bool>(true);

    if (const Array<T>* otherArray = dynamic_cast<const Array<T>* >(&other))
    {
        if (otherArray->size != this->size) return Primitive<bool>(false);

        for (Primitive<int> i; i < otherArray->size; ++i)
        {
            if (*otherArray->data[i.getValue()] != *this->data[i.getValue()]) return Primitive<bool>(false);
        }
        return Primitive<bool>(true);
    }

    return Primitive<bool>(false);
}

template <typename T> requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Primitive<int> Array<T>::getNormalIndex(int index) const
{
    return getNormalIndex(Primitive<int>(index));
}

template <typename T>
requires std::is_arithmetic_v<T> || std::is_base_of_v<Object, T>
Primitive<int> Array<T>::getNormalIndex(Primitive<int> index) const
{
    int sz = static_cast<int>(size.getValue());
    if (index < 0) index += sz;
    if (index < 0 || index >= sz)
        throw std::out_of_range("Index out of bounds");
    return index;
}
