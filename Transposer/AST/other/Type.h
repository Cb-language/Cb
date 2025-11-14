#pragma once
#include <iostream>
#include <string>

// Not a statement
class Type
{
private:
    const std::wstring type;

public:
    explicit Type(const std::wstring& type);
    bool operator==(const Type& other) const;
    bool operator!=(const Type& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Type& t);
};