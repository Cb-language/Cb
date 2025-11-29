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
    const std::string extra; // i.e unsigned/signed
    static const std::unordered_map<std::wstring, std::string> typeMap;
    static const std::unordered_map<std::wstring, std::unordered_set<std::wstring>> castMap;

    static std::wstring getNormalType(const std::wstring& type);
    static std::string getNormalExtra(const std::wstring& type);

public:
    explicit Type(const std::wstring& type);
    bool operator==(const Type& other) const;
    bool operator!=(const Type& other) const;
    bool operator==(const std::wstring &other) const;
    bool operator!=(const std::wstring &other) const;

    bool isNumberable() const;
    bool isStringable() const;

    bool isPrimitive() const;

    std::wstring getType() const;

    std::string translateTypeToCpp() const;
};