#include "LibHelper.h"

const std::vector<std::pair<std::filesystem::path, std::string>> LibHelper::files =
{
    { R"(Array.h)", R"(#pragma once
#include <sstream>

#include "SafePtr.h"
#include <vector>

class Object;

template <typename T>
class Array : public Object
{
public:
	Array(); // default constructor
	explicit Array(size_t size, SafePtr<T> defaultValue = SafePtr<T>());
	Array(const Array& other);
	template <typename U>
	explicit Array(const Array<U>& other, bool isValArray = false);
	~Array() override = default;

	Array& operator=(const Array& other);
	template <typename U>
	Array& operator=(const Array<U>& other);
	Array& operator=(const T& defaultValue);
	template <typename U>
	Array& operator=(const U& defaultValue);

	SafePtr<T>& operator[](int index);
	const SafePtr<T>& operator[](int index) const;

	size_t Length() const { return size; }
	size_t NegLength() const { return static_cast<size_t>(-static_cast<int>(size) - 1); }

	// Only enable add() if T is Object or derived
	template <typename U = T>
	std::enable_if_t<std::is_base_of_v<Object, U>, void>
	add(const U& obj);
	void add(T& value, size_t index = -1);
	void remove(int index);

	Array slice(int start, int stop = -1, int step = 1) const;

	friend std::ostream& operator<<(std::ostream& os, const Array& other)
	{
		bool first = true;
		for (size_t i = 0; i < other.size; i++)
		{
			os << (first ? "" : ", ") << "[" << other[i] << "]";
			first = false;
		}
		return os;
	}

	std::string toString() const override;
	std::unique_ptr<Object> clone() const override;

protected:
	bool equals(const Object& other) const override;

private:
	size_t size = 0;
	SafePtr<T> defaultValueSet = SafePtr<T>();
	std::vector<SafePtr<T>> data;

	int getNormalIndex(int index) const;
};

#include "Array.tpp"
)" },
    { R"(Array.tpp)", R"(#pragma once
#include "Array.h"
#include "Object.h"
#include "String.h"

template <typename T>
Array<T>::Array()
{
    size = 1;
    defaultValueSet(SafePtr<T>());
    data.push_back(defaultValueSet.cloneSafePtr());
}

template <typename T>
Array<T>::Array(size_t size, SafePtr<T> defaultValue)
    : size(size)
{
    defaultValueSet = defaultValue;

    data.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
    }
}

template <typename T>
Array<T>::Array(const Array<T>& other)
{
    size = other.size;
    defaultValueSet = other.defaultValueSet;
    data.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        data.push_back(SafePtr<T>(other.data[i].cloneSafePtr()));
    }
}

template <typename T>
template <typename U>
Array<T>::Array(const Array<U>& other, bool isValArray)
{
    size = other.Length();
    data.reserve(size);

    if constexpr (std::is_same_v<T, std::string>)
    {
        std::ostringstream oss;
        oss << other[0]; // just for default
        defaultValueSet = SafePtr(oss.str());

        for (size_t i = 0; i < size; ++i)
        {
            std::ostringstream ossData;
            ossData << other[i];
            data.push_back(SafePtr(ossData.str()));
        }
    }
    else
    {
        defaultValueSet = SafePtr<T>();
        for (size_t i = 0; i < size; ++i)
        {
            data.push_back(SafePtr<T>(static_cast<T>(*other[i])));
        }
    }
}

template <typename T>
Array<T>& Array<T>::operator=(const Array<T>& other)
{
    if (this == &other)
        return *this;

    size = other.size;
    defaultValueSet = other.defaultValueSet;

    data.clear();
    data.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        data.push_back(other.data[i].cloneSafePtr());
    }
    return *this;
}

template <typename T>
template <typename U>
Array<T>& Array<T>::operator=(const Array<U>& other)
{
    size = other.Length();
    data.clear();
    data.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            std::ostringstream oss;
            oss << *other[i];
            data.push_back(SafePtr<std::string>(oss.str()));
        }
        else
        {
            data.push_back(SafePtr<T>(static_cast<T>(*other[i])));
        }
    }

    defaultValueSet = data[0].clonePtr();
    return *this;
}

template <typename T>
Array<T>& Array<T>::operator=(const T& defaultValue)
{
    data.clear();
    data.reserve(size);

    defaultValueSet = SafePtr<T>(defaultValue);
    for (size_t i = 0; i < size; ++i)
    {
        data.push_back(defaultValueSet.clone());
    }
    return *this;
}

template <typename T>
template <typename U>
Array<T>& Array<T>::operator=(const U& defaultValue)
{
    data.clear();
    data.reserve(size);

    if constexpr (std::is_same_v<T, std::string>)
    {
        std::ostringstream oss;
        oss << defaultValue;
        defaultValueSet = SafePtr<std::string>(oss.str());
    }
    else
    {
        defaultValueSet = SafePtr<T>(static_cast<T>(defaultValue));
    }

    for (size_t i = 0; i < size; ++i)
    {
        data.push_back(defaultValueSet.cloneSafePtr());
    }

    return *this;
}

template <typename T>
SafePtr<T>& Array<T>::operator[](int index)
{
    index = getNormalIndex(index);
    return data[index];
}

template <typename T>
const SafePtr<T>& Array<T>::operator[](int index) const
{
    index = getNormalIndex(index);
    return data[index];
}

template <typename T>
template <typename U>
std::enable_if_t<std::is_base_of_v<Object, U>, void> Array<T>::add(const U& obj)
{
    data.push_back(SafePtr<T>(obj.clone()));
    ++size;
}

template <typename T>
void Array<T>::add(T& value, size_t index)
{
    if (index == static_cast<size_t>(-1) || index >= size)
        index = size;

    data.insert(data.begin() + index, SafePtr<T>(value));
    ++size;
}

template <typename T>
void Array<T>::remove(int index)
{
    index = getNormalIndex(index);
    data.erase(data.begin() + index);
    --size;
}

template <typename T>
Array<T> Array<T>::slice(int start, int stop, int step) const
{
    if (step == 0)
        throw std::out_of_range("Step cannot be zero");

    int sz = static_cast<int>(size);
    if (start < 0) start += sz;
    if (stop < 0) stop += sz;
    if (stop == -1) stop = sz;

    Array<T> result;
    result.data.clear();

    for (int i = start; (step > 0 ? i < stop : i > stop); i += step)
    {
        result.data.push_back(data[i].clone());
        ++result.size;
    }

    result.defaultValueSet = defaultValueSet.clone();
    return result;
}

template <typename T>
std::string Array<T>::toString() const
{
    std::string str = "[";
    bool first = true;

    for (size_t i = 0; i < size; ++i)
    {
        if (!first) str += ", ";
        str += data[i]->toString();
        first = false;
    }

    str += "]";
    return str;
}

template <typename T>
std::unique_ptr<Object> Array<T>::clone() const
{
    return std::make_unique<Array<T>>(*this);
}

template <typename T>
bool Array<T>::equals(const Object& other) const
{
    if (this == &other) return true;

    if (const Array<T>* otherArray = dynamic_cast<const Array<T>* >(&other))
    {
        if (otherArray->size != this->size) return false;

        for (size_t i = 0; i < otherArray->size; ++i)
        {
            if (*otherArray->data[i] != *this->data[i]) return false;
        }
        return true;
    }

    return false;
}

template <typename T>
int Array<T>::getNormalIndex(int index) const
{
    int sz = static_cast<int>(size);
    if (index < 0) index += sz;
    if (index < 0 || index >= sz)
        throw std::out_of_range("Index out of bounds");
    return index;
}
)" },
    { R"(Object.cpp)", R"(#include "Object.h"

bool Object::operator==(const Object& other) const
{
    return this == &other || equals(other);
}

bool Object::operator!=(const Object& other) const
{
    return !(*this == other);
}

bool Object::equals(const Object& other) const
{
    return false;
}

std::ostream& operator<<(std::ostream& os, const Object& obj)
{
    os << obj.toString();
    return os;
}
)" },
    { R"(Object.h)", R"(#pragma once
#include <memory>
#include <sstream>

class Object
{
public:
    Object() = default;
    virtual ~Object() = default;
    Object(const Object&) = default;
    bool operator==(const Object& other) const;
    bool operator!=(const Object& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Object& obj);
    virtual std::string toString() const = 0;
    virtual std::unique_ptr<Object> clone() const = 0;

protected:
    virtual bool equals(const Object& other) const;
};
)" },
    { R"(Primitive.h)", R"(#pragma once
#include "Object.h"

class PrimitiveBase : public Object
{
public:
    virtual double getNumericValue() const = 0;
};

template <typename T>
class Primitive : public PrimitiveBase
{
private:
    T value;

public:
    double getNumericValue() const override;
    explicit Primitive(const T& value = T());
    Primitive(const Primitive& other);
    Primitive& operator=(const Primitive& other);

    // ReSharper disable once CppEnforceOverridingFunctionStyle
    Primitive& operator=(const T& other);

    template <typename U>
    Primitive& operator=(const Primitive<U>& other);

    template <typename U>
    Primitive& operator=(const U& other);

    Primitive<T>& operator=(const Object& other);

    std::string toString() const override;
    std::unique_ptr<Object> clone() const override;
    T getValue() const;

    Primitive operator+(const Primitive& other) const;
    Primitive operator-(const Primitive& other) const;
    Primitive operator*(const Primitive& other) const;
    Primitive operator/(const Primitive& other) const;
    Primitive operator%(const Primitive& other) const;

    Primitive& operator+=(const Primitive& other);
    Primitive& operator-=(const Primitive& other);
    Primitive& operator*=(const Primitive& other);
    Primitive& operator/=(const Primitive& other);
    Primitive& operator%=(const Primitive& other);


    Primitive operator+(const T& other) const;
    Primitive operator-(const T& other) const;
    Primitive operator*(const T& other) const;
    Primitive operator/(const T& other) const;
    Primitive operator%(const T& other) const;

    Primitive& operator+=(const T& other);
    Primitive& operator-=(const T& other);
    Primitive& operator*=(const T& other);
    Primitive& operator/=(const T& other);
    Primitive& operator%=(const T& other);

    template <typename U>
    Primitive operator+(const Primitive<U>& other) const;
    template <typename U>
    Primitive operator-(const Primitive<U>& other) const;
    template <typename U>
    Primitive operator*(const Primitive<U>& other) const;
    template <typename U>
    Primitive operator/(const Primitive<U>& other) const;
    template <typename U>
    Primitive operator%(const Primitive<U>& other) const;

    template <typename U>
    Primitive& operator+=(const Primitive<U>& other);
    template <typename U>
    Primitive& operator-=(const Primitive<U>& other);
    template <typename U>
    Primitive& operator*=(const Primitive<U>& other);
    template <typename U>
    Primitive& operator/=(const Primitive<U>& other);
    template <typename U>
    Primitive& operator%=(const Primitive<U>& other);


    template <typename U>
    Primitive operator+(const U& other) const;
    template <typename U>
    Primitive operator-(const U& other) const;
    template <typename U>
    Primitive operator*(const U& other) const;
    template <typename U>
    Primitive operator/(const U& other) const;
    template <typename U>
    Primitive operator%(const U& other) const;

    template <typename U>
    Primitive& operator+=(const U& other);
    template <typename U>
    Primitive& operator-=(const U& other);
    template <typename U>
    Primitive& operator*=(const U& other);
    template <typename U>
    Primitive& operator/=(const U& other);
    template <typename U>
    Primitive& operator%=(const U& other);

    Primitive<bool> operator||(const Primitive<bool>& other);
    Primitive<bool> operator&&(const Primitive<bool>& other);
    Primitive<bool> operator||(bool other);
    Primitive<bool> operator&&(bool other);
    explicit operator bool() const;

protected:
    bool equals(const Object& other) const override;
};

#include "Primitive.tpp"
)" },
    { R"(Primitive.tpp)", R"(#pragma once
#include "Primitive.h"

template <typename T>
double Primitive<T>::getNumericValue() const
{
    return static_cast<double>(value);
}

template <typename T>
Primitive<T>::Primitive(const T& value) : value(value)
{
}

template <typename T>
Primitive<T>::Primitive(const Primitive& other) : value(other.value)
{
}

template <typename T>
Primitive<T>& Primitive<T>::operator=(const Primitive& other)
{
    if (this == &other) return *this;

    value = other.value;

    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator=(const T& other)
{
    value = other;

    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator=(const Primitive<U>& other)
{
    value = static_cast<T>(other.getValue());

    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator=(const U& other)
{
    static_cast<T>(other);

    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator=(const Object& other)
{
    if (this == &other) return *this;

    if (const Primitive<T>* p = dynamic_cast<const Primitive<T>*>(&other))
    {
        value = p->value;
    }
    else
    {
        throw std::logic_error("Cannot cast");
    }

    return *this;
}

template <typename T>
bool Primitive<T>::equals(const Object& other) const
{
    if (this == &other) return true;

    if (const PrimitiveBase* pBase = dynamic_cast<const PrimitiveBase*>(&other))
    {
        return getNumericValue() == pBase->getNumericValue();
    }

    return false;
}

// ------ non-member func for T op Primitive
template <typename T>
Primitive<T> operator+(T left, const Primitive<T>& right)
{
    return Primitive(left) + right;
}

template <typename T>
Primitive<T> operator-(T left, const Primitive<T>& right)
{
    return Primitive(left) - right;
}

template <typename T>
Primitive<T> operator*(T left, const Primitive<T>& right)
{
    return Primitive(left) * right;
}

template <typename T>
Primitive<T> operator/(T left, const Primitive<T>& right)
{
    return Primitive(left) / right;
}

template <typename T>
Primitive<T> operator%(T left, const Primitive<T>& right)
{
    return Primitive(left) % right;
}

template <typename T, typename U>
Primitive<T> operator+(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) + right;
}

template <typename T, typename U>
Primitive<T> operator-(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) - right;
}

template <typename T, typename U>
Primitive<T> operator*(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) * right;
}

template <typename T, typename U>
Primitive<T> operator/(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) / right;
}

template <typename T, typename U>
Primitive<T> operator%(U left, const Primitive<T>& right)
{
    return Primitive<U>(left) % right;
}

// -------------

template <typename T>
std::string Primitive<T>::toString() const
{
    if constexpr (std::is_same_v<T,bool>) return value ? "cres" : "demen";
    return std::to_string(value);
}

template <typename T>
std::unique_ptr<Object> Primitive<T>::clone() const
{
    return std::make_unique<Primitive>(value);
}

template <typename T>
T Primitive<T>::getValue() const
{
    return value;
}

template <typename T>
Primitive<T> Primitive<T>::operator+(const Primitive& other) const
{
    return Primitive(value + other.value);
}

template <typename T>
Primitive<T> Primitive<T>::operator-(const Primitive& other) const
{
    return Primitive(value - other.value);
}

template <typename T>
Primitive<T> Primitive<T>::operator*(const Primitive& other) const
{
    return Primitive(value * other.value);
}

template <typename T>
Primitive<T> Primitive<T>::operator/(const Primitive& other) const
{
    return Primitive(value / other.value);
}

template <typename T>
Primitive<T> Primitive<T>::operator%(const Primitive& other) const
{
    return Primitive(value % other.value);
}

template <typename T>
Primitive<T>& Primitive<T>::operator+=(const Primitive& other)
{
    value += other.value;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator-=(const Primitive& other)
{
    value -= other.value;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator*=(const Primitive& other)
{
    value *= other.value;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator/=(const Primitive& other)
{
    value /= other.value;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator%=(const Primitive& other)
{
    value %= other.value;
    return *this;
}

template <typename T>
Primitive<T> Primitive<T>::operator+(const T& other) const
{
    return Primitive<T>(value + other);
}

template <typename T>
Primitive<T> Primitive<T>::operator-(const T& other) const
{
    return Primitive<T>(value - other);
}

template <typename T>
Primitive<T> Primitive<T>::operator*(const T& other) const
{
    return Primitive<T>(value * other);
}

template <typename T>
Primitive<T> Primitive<T>::operator/(const T& other) const
{
    return Primitive<T>(value / other);
}

template <typename T>
Primitive<T> Primitive<T>::operator%(const T& other) const
{
    return Primitive<T>(value % other);
}

template <typename T>
Primitive<T>& Primitive<T>::operator+=(const T& other)
{
    value += other;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator-=(const T& other)
{
    value -= other;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator*=(const T& other)
{
    value *= other;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator/=(const T& other)
{
    value /= other;
    return *this;
}

template <typename T>
Primitive<T>& Primitive<T>::operator%=(const T& other)
{
    value %= other;
    return *this;
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator+(const Primitive<U>& other) const
{
    return Primitive<T>(value + static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator-(const Primitive<U>& other) const
{
    return Primitive<T>(value - static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator*(const Primitive<U>& other) const
{
    return Primitive<T>(value * static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator/(const Primitive<U>& other) const
{
    return Primitive<T>(value / static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator%(const Primitive<U>& other) const
{
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "Modulo only allowed for integral types");
    return Primitive<T>(value % static_cast<T>(other.getValue()));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator+(const U& other) const
{
    return Primitive<T>(value + static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator-(const U& other) const
{
    return Primitive<T>(value - static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator*(const U& other) const
{
    return Primitive<T>(value * static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator/(const U& other) const
{
    return Primitive<T>(value / static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T> Primitive<T>::operator%(const U& other) const
{
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "Modulo only allowed for integral types");
    return Primitive<T>(value % static_cast<T>(other));
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator+=(const Primitive<U>& other)
{
    value += static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator-=(const Primitive<U>& other)
{
    value -= static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator*=(const Primitive<U>& other)
{
    value *= static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator/=(const Primitive<U>& other)
{
    value /= static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator%=(const Primitive<U>& other)
{
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "Modulo only allowed for integral types");
    value %= static_cast<T>(other.getValue());
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator+=(const U& other)
{
    value += static_cast<T>(other);
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator-=(const U& other)
{
    value -= static_cast<T>(other);
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator*=(const U& other)
{
    value *= static_cast<T>(other);
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator/=(const U& other)
{
    value /= static_cast<T>(other);
    return *this;
}

template <typename T>
template <typename U>
Primitive<T>& Primitive<T>::operator%=(const U& other)
{
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "Modulo only allowed for integral types");
    value %= static_cast<T>(other);
    return *this;
}

template <typename T>
Primitive<bool> Primitive<T>::operator||(const Primitive<bool>& other)
{
    return Primitive<bool>(value || other.value);
}

template <typename T>
Primitive<bool> Primitive<T>::operator&&(const Primitive<bool>& other)
{
    return Primitive<bool>(value && other.value);
}

template <typename T>
Primitive<bool> Primitive<T>::operator||(bool other)
{
    return Primitive<bool>(value || other);
}

template <typename T>
Primitive<bool> Primitive<T>::operator&&(bool other)
{
    return Primitive<bool>(value && other);
}

template <typename T>
Primitive<T>::operator bool() const
{
    if constexpr (std::is_same_v<T, bool>) return value;

    return value == T();
}
)" },
    { R"(SafePtr.h)", R"(#pragma once
#include <memory>

#include "Object.h"

template <typename T>
class SafePtr : public Object
{
private:
    std::unique_ptr<T> ptr;
public:
    SafePtr();
    explicit SafePtr(const std::unique_ptr<T>& ptr);
    explicit SafePtr(const T& t);
    SafePtr(const SafePtr& other);

    template <typename U>
    explicit SafePtr(std::unique_ptr<U>& ptr);

    template <typename U>
    explicit SafePtr(const U& u);

    template <typename U>
    explicit SafePtr(const SafePtr<U>& other);

    SafePtr& operator=(const T& t);
    SafePtr& operator=(const SafePtr& t);

    template <typename U>
    SafePtr& operator=(const U& u);

    template <typename U>
    SafePtr& operator=(const SafePtr<U>& u);

    SafePtr cloneSafePtr() const;

    T* operator->() const;
    T& operator*() const;
    T* get() const;
    std::unique_ptr<T> clonePtr() const;

    friend std::ostream& operator<<(std::ostream& os, const SafePtr<T>& safePtr)
    {
        os << *(safePtr.ptr.get());
        return os;
    }

    ~SafePtr() override = default;

    std::string toString() const override;
    std::unique_ptr<Object> clone() const override;

protected:
    bool equals(const Object& other) const override;
};

#include "SafePtr.tpp"
)" },
    { R"(SafePtr.tpp)", R"(#pragma once
#include <algorithm>

#include "Object.h"
#include "SafePtr.h"
#include "String.h"

template <typename T>
std::unique_ptr<T> SafePtr<T>::clonePtr() const
{
    return std::unique_ptr<T>(ptr->clone());
}

template <typename T>
std::string SafePtr<T>::toString() const
{
    return ptr->toString();
}

template <typename T>
std::unique_ptr<Object> SafePtr<T>::clone() const
{
    return std::make_unique<SafePtr<T>>(*ptr);
}

template <typename T>
bool SafePtr<T>::equals(const Object& other) const
{
    if (this == &other)

    if (auto p = dynamic_cast<const SafePtr<T>*>(&other))
    {
        return *this == *p;
    }

    if (auto p = dynamic_cast<const T*>(&other))
    {
        return *ptr == *p;
    }

    return false;
}

template <typename T>
SafePtr<T>::SafePtr()
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");

    if constexpr (std::is_same_v<T, Object>)
    {
        ptr = std::make_unique<String>();
    }
    else
    {
        ptr = std::make_unique<T>();
    }
}

template <typename T>
SafePtr<T>::SafePtr(const std::unique_ptr<T>& ptr)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");

    this->ptr = std::unique_ptr<T>(ptr.get());
}

template <typename T>
SafePtr<T>::SafePtr(const T& t)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");

    if constexpr (std::is_same_v<T, Object>)
    {
        ptr = std::move(t.clone());
    }
    else
    {
        ptr = std::make_unique<T>(t);
    }
}

template <typename T>
SafePtr<T>::SafePtr(const SafePtr& other)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");
    ptr = std::move(other.clonePtr());
}

template <typename T>
template <typename U>
SafePtr<T>::SafePtr(std::unique_ptr<U>& ptr)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");
    static_assert(std::is_base_of_v<Object, U>, "non-Object-derived type created");
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");


    this->ptr = std::move(ptr.clone());
}

template <typename T>
template <typename U>
SafePtr<T>::SafePtr(const U& u)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");
    static_assert(std::is_base_of_v<Object, U>, "non-Object-derived type created");
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");

    ptr = std::make_unique<T>(dynamic_cast<T>(u));
}

template <typename T>
template <typename U>
SafePtr<T>::SafePtr(const SafePtr<U>& other)
{
    static_assert(std::is_base_of_v<Object, T>, "non-Object-derived type created");
    static_assert(std::is_base_of_v<Object, U>, "non-Object-derived type created");
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");

    ptr = std::unique_ptr<T>(dynamic_cast<T*>(other.ptr->clone()));
}

template <typename T>
SafePtr<T>& SafePtr<T>::operator=(const T& t)
{
    ptr = std::make_unique<T>(t);
    return *this;
}

template <typename T>
SafePtr<T>& SafePtr<T>::operator=(const SafePtr& t)
{
    ptr = std::move(t.clonePtr());
    return *this;
}

template <typename T>
template <typename U>
SafePtr<T>& SafePtr<T>::operator=(const U& u)
{
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");

    if constexpr (std::is_same_v<T, Object>)
    {
        ptr = u.clone();
    }
    else
    {
        ptr = std::make_unique<T>(dynamic_cast<T>(u));
    }

    return *this;
}

template <typename T>
template <typename U>
SafePtr<T>& SafePtr<T>::operator=(const SafePtr<U>& u)
{
    static_assert(std::is_convertible_v<U*, T*>, "Cannot cast");

    ptr = u.clonePtr();
    return *this;
}

template <typename T>
SafePtr<T> SafePtr<T>::cloneSafePtr() const
{
    return SafePtr<T>(*this);
}

template <typename T>
T* SafePtr<T>::operator->() const
{
    return ptr.get();
}

template <typename T>
T& SafePtr<T>::operator*() const
{
    return *ptr;
}

template <typename T>
T* SafePtr<T>::get() const
{
    return ptr.get();
})" },
    { R"(String.cpp)", R"(#include "String.h"

String::String()
= default;

String::String(const std::string& basicString) : value(basicString)
{
}

String::String(const String& string) : value(string.value)
{
}

String::String(const char* basicCharPtr) : value(basicCharPtr)
{
}

std::string String::toString() const
{
    return value;
}

std::unique_ptr<Object> String::clone() const
{
    return std::make_unique<String>(value);
}

String& String::operator=(const String& other)
{
    value = other.value;
    return *this;
}

String& String::operator=(const Object& other)
{
    if (this == &other) return *this;

    if (const String* p = dynamic_cast<const String*>(&other))
    {
        value = p->value;
        return *this;
    }

    throw std::logic_error("Cannot cast");
}

String& String::operator=(const char* basicCharPtr)
{
    value = basicCharPtr;
    return *this;
}

String String::operator+(const char* basicCharPtr) const
{
    return String(value + basicCharPtr);
}

String& String::operator+=(const char* basicCharPtr)
{
    value += basicCharPtr;
    return *this;
}

String String::operator+(const Object& obj) const
{
    return String(value + obj.toString());
}

String& String::operator+=(const Object& obj)
{
    value += obj.toString();
    return *this;
}

bool String::equals(const Object& other) const
{
    if (this == &other) return true;

    if (const String* p = dynamic_cast<const String*>(&other))
    {
        return value == p->value;
    }

    return false;
}

std::ostream& operator<<(std::ostream& os, const String& str)
{
    os << '\"' << str.value << '\"';
    return os;
}
)" },
    { R"(String.h)", R"(#pragma once
#include "Object.h"
#include "Primitive.h"

class String : public Object
{
private:
    std::string value;

public:
    String();
    explicit String(const std::string& basicString);
    explicit String(const String& string);
    explicit String(const char* basicCharPtr);
    std::string toString() const override;
    std::unique_ptr<Object> clone() const override;
    String& operator=(const String& other);
    String& operator=(const Object& other);
    String& operator=(const char* basicCharPtr);
    String operator+(const char* basicCharPtr) const;
    String& operator+=(const char* basicCharPtr);

    String operator+(const Object& obj) const;
    String& operator+=(const Object& obj);

    template <typename T>
    String& operator=(const Primitive<T>& other)
    {
        value = other.toString();
        return *this;
    }

    template <typename T>
    String operator*(const Primitive<T>& p) const
    {
        std::string str;
        for (int i = 0; i < static_cast<int>(p.getValue()); i++)
        {
            str += value;
        }

        return String(str);
    }

    template <typename T>
    String& operator*=(const Primitive<T>& p)
    {
        *this = *this * p;
        return *this;
    }

    template <typename T>
    String operator*(const T& p) const
    {
        if constexpr (!std::is_arithmetic_v<T>) throw std::logic_error("Cannot cast");
        std::string str;
        for (int i = 0; i < p.getValue(); i++)
        {
            str += value;
        }

        return String(str);
    }

    template <typename T>
    String& operator*=(const T& p)
    {
        if constexpr (!std::is_arithmetic_v<T>) throw std::logic_error("Cannot cast");
        std::string str;
        for (int i = 0; i < static_cast<int>(p); i++)
        {
            str += value;
        }

        value = str;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const String& str);


protected:
    bool equals(const Object& other) const override;
};
)" },
    { R"(Utils.h)", R"(#pragma once
#include "Object.h"

class Utils
{
public:
    template <typename T>
    static T& cast(Object& other);

    template <typename T>
    static T& cast(SafePtr<Object>& other);
};

#include "Utils.tpp")" },
    { R"(Utils.tpp)", R"(#pragma once
#include "Primitive.h"
#include "Utils.h"

template <typename T>
T& Utils::cast(Object& other)
{
    if (auto p = dynamic_cast<T*>(other))
    {
        return *p;
    }

    throw std::logic_error("Illegal cast");
}

template <typename T>
T& Utils::cast(SafePtr<Object>& other)
{
    if (auto o = other.get())
    {
        if (auto p = dynamic_cast<T*>(o))
            return *p;
    }

    throw std::logic_error("Illegal cast");
}
)" },
};

void LibHelper::write(const std::filesystem::path& dir)
{
    std::filesystem::create_directories(dir / "includes");
    for (const auto& [path, content] : files)
    {
        const std::filesystem::path outPath = dir / "includes" / path;
        std::ofstream out(outPath, std::ios::binary);
        out << content;
    }
}
