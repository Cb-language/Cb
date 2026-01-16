#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "IType.h"

class Type : public IType
{
private:
    std::string extra; // i.e unsigned/signed
    static const std::unordered_map<std::wstring, std::string> typeMap;
    static const std::unordered_map<std::wstring, std::unordered_set<std::wstring>> castMap;

    static std::wstring getNormalType(const std::wstring& type);
    static std::string getNormalExtra(const std::wstring& type);

public:
    explicit Type(const std::wstring& type);
    explicit Type(Type* other);
    Type(const std::wstring& type, const std::string& extra);

    bool operator==(const IType& other) const override;
    bool operator!=(const IType& other) const override;
    bool operator==(const std::wstring &other) const override;
    bool operator!=(const std::wstring &other) const override;

    bool isNumberable() const override;
    bool isStringable() const override;

    bool isPrimitive() const override;

    std::wstring getType() const override;

    std::string translateTypeToCpp() const override;

    std::unique_ptr<IType> copy() const override;
};
