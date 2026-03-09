#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "IType.h"

enum class Primitive
{
    TYPE_FLAT,    // signed
    TYPE_SHARP,   // unsigned
    TYPE_DEGREE,  // int
    TYPE_FREQ,    // float
    TYPE_NOTE,    // char
    TYPE_MUTE,    // bool
    TYPE_BAR,     // string
    TYPE_RIFF,    // array
    TYPE_FERMATA, // void
};

enum class SignType
{
    SIGNED,
    UNSIGNED,
};

class PrimitiveType : public IType
{
private:
    Primitive type;
    SignType signType; // i.e unsigned/signed
    static const std::unordered_map<Primitive, std::string> typeMap;
    static const std::unordered_map<Primitive, std::unordered_set<Primitive>> castMap;

public:
    explicit PrimitiveType(Primitive type);
    PrimitiveType(Primitive type, SignType signType);

    bool operator==(const IType& other) const override;
    bool operator!=(const IType& other) const override;

    bool isNumberable() const override;
    bool isStringable() const override;

    bool isPrimitive() const override;

    std::string translateTypeToCpp() const override;

    std::unique_ptr<IType> copy() const override;

    std::string toString() const override;
};
