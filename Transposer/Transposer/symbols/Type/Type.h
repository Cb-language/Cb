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
    static const std::unordered_map<std::string, std::string> typeMap;
    static const std::unordered_map<std::string, std::unordered_set<std::string>> castMap;

    static std::string getNormalType(const std::string& type);
    static std::string getNormalExtra(const std::string& type);

public:
    explicit Type(const std::string& type);
    explicit Type(Type* other);
    Type(const std::string& type, const std::string& extra);

    bool operator==(const IType& other) const override;
    bool operator!=(const IType& other) const override;
    bool operator==(const std::string &other) const override;
    bool operator!=(const std::string &other) const override;

    bool isNumberable() const override;
    bool isStringable() const override;

    bool isPrimitive() const override;

    std::string getType() const override;

    std::string translateTypeToCpp() const override;

    std::unique_ptr<IType> copy() const override;
};
