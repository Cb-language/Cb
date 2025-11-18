#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Not a statement
class Type
{
private:
    const std::wstring type;
    static const std::unordered_map<std::wstring, std::string> typeMap;
    static const std::unordered_map<std::wstring, std::unordered_set<std::wstring>> castMap;

    std::wstring normalizedType() const;

public:
    explicit Type(std::wstring  type);
    bool operator==(const Type& other) const;
    bool operator!=(const Type& other) const;
    std::wstring getType() const;

    std::string translateTypeToCpp() const;
};