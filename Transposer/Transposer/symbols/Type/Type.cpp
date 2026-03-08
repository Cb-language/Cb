#include "Type.h"
#include <utility>

const std::unordered_map<std::string, std::string> Type::typeMap = {
    {"bar",     "std::string"}, // String
    {"note",    "char"},        // Char
    {"mute",    "bool"},        // Bool
    {"freq",    "double"},      // Double
    {"degree",  "int"},         // Int
    //{"scale",   ""}, // Array - TODO: add this in sprint 3
    {"fermata", "void"}         // Void
};

const std::unordered_map<std::string, std::unordered_set<std::string>> Type::castMap = {
    {"bar",     {"bar"}},                   // String -> only String
    {"note",    {"note", "degree", "freq"}}, // Char -> Int, Double
    {"mute",    {"mute", "degree", "freq"}}, // Bool -> Int, Double
    {"degree",  {"degree", "note", "freq"}}, // Int -> Char, Double
    {"freq",    {"freq", "degree", "note"}}, // Double -> Int, Char
    //{"scale",   {"scale"}},               // Array - TODO: add this in sprint 3
    {"fermata", {}}                          // Void -> nothing
};

Type::Type(const std::string& type) : IType(getNormalType(type)), extra(getNormalExtra(type))
{
}

Type::Type(Type* other) : IType(other), extra(other->extra)
{
}

Type::Type(const std::string& type, const std::string& extra) : IType(type), extra(extra)
{
}

std::string Type::getNormalType(const std::string& type)
{
    if (type.rfind("flat ", 0) == 0)
    {
        return type.substr(5);
    }
    if (type.rfind("sharp ", 0) == 0)
    {
        return type.substr(6);
    }

    return type;
}

std::string Type::getNormalExtra(const std::string& type)
{
    if (type.rfind("flat ", 0) == 0)
    {
        return "signed";
    }
    if (type.rfind("sharp ", 0) == 0)
    {
        return "unsigned";
    }

    return "";
}

bool Type::operator==(const IType& other) const
{
    const std::string type = other.getType();
    const auto it = castMap.find(type);
    if (it == castMap.end()) return this->type == type; // for future (class)
    return it->second.contains(this->type);
}

bool Type::operator!=(const IType& other) const
{
    return !(*this == other);
}

bool Type::operator==(const std::string& other) const
{
    const auto it = castMap.find(this->type);
    if (it == castMap.end()) return this->type == other;
    return it->second.contains(other);
}

bool Type::operator!=(const std::string& other) const
{
    return !(*this == other);
}

bool Type::isNumberable() const
{
    return *this == "degree";
}

bool Type::isStringable() const
{
    return *this == "bar";
}

bool Type::isPrimitive() const
{
    return true;
}

std::string Type::getType() const
{
    return type;
}

std::string Type::translateTypeToCpp() const
{
    std::string ret;

    if (type == "bar")
    {
        ret = "String";
    }
    else if (type == "fermata")
    {
        return "void";
    }
    else
    {
        std::string typeStr;
        if (!extra.empty())
        {
            typeStr = extra + " ";
        }

        typeStr += Type::typeMap.at(type);
        ret = "Primitive<" + typeStr + ">";
    }

    return ret;
}

std::unique_ptr<IType> Type::copy() const
{
    return std::make_unique<Type>(type, extra);
}
