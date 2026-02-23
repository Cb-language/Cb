#include "Type.h"
#include <utility>

const std::unordered_map<std::wstring, std::string> Type::typeMap = {
    {L"bar",     "std::string"}, // String
    {L"note",    "char"},        // Char
    {L"mute",    "bool"},        // Bool
    {L"freq",    "double"},      // Double
    {L"degree",  "int"},         // Int
    //{L"scale",   ""}, // Array - TODO: add this in sprint 3
    {L"fermata", "void"}         // Void
};

const std::unordered_map<std::wstring, std::unordered_set<std::wstring>> Type::castMap = {
    {L"bar",     {L"bar"}},                   // String -> only String
    {L"note",    {L"note", L"degree", L"freq"}}, // Char -> Int, Double
    {L"mute",    {L"mute", L"degree", L"freq"}}, // Bool -> Int, Double
    {L"degree",  {L"degree", L"note", L"freq"}}, // Int -> Char, Double
    {L"freq",    {L"freq", L"degree", L"note"}}, // Double -> Int, Char
    //{L"scale",   {L"scale"}},               // Array - TODO: add this in sprint 3
    {L"fermata", {}}                          // Void -> nothing
};

Type::Type(const std::wstring& type) : IType(getNormalType(type)), extra(getNormalExtra(type))
{
}

Type::Type(Type* other) : IType(other), extra(other->extra)
{
}

Type::Type(const std::wstring& type, const std::string& extra) : IType(type), extra(extra)
{
}

std::wstring Type::getNormalType(const std::wstring& type)
{
    if (type.rfind(L"flat ", 0) == 0)
    {
        return type.substr(5);
    }
    if (type.rfind(L"sharp ", 0) == 0)
    {
        return type.substr(6);
    }

    return type;
}

std::string Type::getNormalExtra(const std::wstring& type)
{
    if (type.rfind(L"flat ", 0) == 0)
    {
        return "signed";
    }
    if (type.rfind(L"sharp ", 0) == 0)
    {
        return "unsigned";
    }

    return "";
}

bool Type::operator==(const IType& other) const
{
    const std::wstring type = other.getType();
    const auto it = castMap.find(type);
    if (it == castMap.end()) return this->type == type; // for future (class)
    return it->second.contains(this->type);
}

bool Type::operator!=(const IType& other) const
{
    return !(*this == other);
}

bool Type::operator==(const std::wstring& other) const
{
    const auto it = castMap.find(this->type);
    if (it == castMap.end()) return this->type == other;
    return it->second.contains(other);
}

bool Type::operator!=(const std::wstring& other) const
{
    return !(*this == other);
}

bool Type::isNumberable() const
{
    return *this == L"degree";
}

bool Type::isStringable() const
{
    return *this == L"bar";
}

bool Type::isPrimitive() const
{
    return true;
}

std::wstring Type::getType() const
{
    return type;
}

std::string Type::translateTypeToCpp() const
{
    std::string ret;

    if (type == L"bar")
    {
        ret = "String";
    }
    else if (type == L"fermata")
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
