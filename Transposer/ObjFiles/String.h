#pragma once
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
    Primitive<bool> equals(const Object& other) const override;
};
