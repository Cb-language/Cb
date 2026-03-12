#include "PrimitiveType.h"
#include <utility>

const std::unordered_map<Primitive, std::string> PrimitiveType::typeMap = {
    {Primitive::TYPE_BAR,     "std::string"}, // String
    {Primitive::TYPE_NOTE,    "char"},        // Char
    {Primitive::TYPE_MUTE,    "bool"},        // Bool
    {Primitive::TYPE_FREQ,    "double"},      // Double
    {Primitive::TYPE_DEGREE,  "int"},         // Int
    {Primitive::TYPE_FERMATA, "void"}         // Void
};

const std::unordered_map<Primitive, std::unordered_set<Primitive>> PrimitiveType::castMap = {
    {Primitive::TYPE_BAR, {Primitive::TYPE_BAR}},                   // String -> only String
    {Primitive::TYPE_NOTE, {Primitive::TYPE_NOTE, Primitive::TYPE_DEGREE, Primitive::TYPE_FREQ}}, // Char -> Int, Double
    {Primitive::TYPE_MUTE, {Primitive::TYPE_MUTE, Primitive::TYPE_DEGREE, Primitive::TYPE_FREQ}},    // Bool -> Int, Double
    {Primitive::TYPE_FREQ, {Primitive::TYPE_DEGREE, Primitive::TYPE_DEGREE, Primitive::TYPE_FREQ}},      // Int -> Char, Double
    {Primitive::TYPE_DEGREE, {Primitive::TYPE_FREQ, Primitive::TYPE_DEGREE, Primitive::TYPE_FREQ}},  // Double -> Int, Char
    {Primitive::TYPE_FERMATA, {}}                                      // Void -> nothing
};

PrimitiveType::PrimitiveType(const Primitive type) : type(type), signType(SignType::SIGNED)
{
}

PrimitiveType::PrimitiveType(const Primitive type, const SignType signType) : type(type), signType(signType)
{
}

bool PrimitiveType::operator==(const IType& other) const
{
    const auto casted = dynamic_cast<const PrimitiveType*>(&other);
    if (!casted) return false;

    if (casted->type == this->type)
    {
        return true;
    }

    const auto it = castMap.find(type);
    return it->second.contains(this->type);
}

bool PrimitiveType::operator!=(const IType& other) const
{
    return !(*this == other);
}

bool PrimitiveType::isNumberable() const
{
    return type == Primitive::TYPE_DEGREE;
}

bool PrimitiveType::isStringable() const
{
    return type == Primitive::TYPE_BAR;
}

bool PrimitiveType::isPrimitive() const
{
    return true;
}

std::string PrimitiveType::translateTypeToCpp() const
{
    std::string ret;

    if (type == Primitive::TYPE_BAR)
    {
        ret = "String";
    }
    else if (type == Primitive::TYPE_FERMATA)
    {
        return "void";
    }
    else
    {
        std::string typeStr;
        if (signType == SignType::UNSIGNED)
        {
            typeStr = "unsigned ";
        }

        typeStr += PrimitiveType::typeMap.at(type);
        ret = "Primitive<" + typeStr + ">";
    }

    return ret;
}

std::unique_ptr<IType> PrimitiveType::copy() const
{
    return std::make_unique<PrimitiveType>(type, signType);
}

std::string PrimitiveType::toString() const
{
    switch (type)
    {
        case Primitive::TYPE_BAR:
        return "bar";
        case Primitive::TYPE_NOTE:
        return "note";
        case Primitive::TYPE_MUTE:
        return "mute";
        case Primitive::TYPE_FREQ:
        return "freq";
        case Primitive::TYPE_DEGREE:
        return "degree";
        case Primitive::TYPE_FERMATA:
        return "fermata";

    default: return "";
    }
}
