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

std::wstring Type::normalizedType() const
{
    if (type.rfind(L"flat ", 0) == 0)
        return type.substr(5);
    if (type.rfind(L"sharp ", 0) == 0)
        return type.substr(6);
    return type;
}

Type::Type(std::wstring  type) : type(std::move(type))
{
}

bool Type::operator==(const Type& other) const
{
    const std::wstring t = normalizedType();
    const std::wstring tOther = other.normalizedType();

    const auto it = castMap.find(tOther);
    if (it == castMap.end()) return t == tOther; // for future (class)
    return it->second.contains(t);
}

bool Type::operator!=(const Type& other) const
{
    return !(*this == other);
}

std::wstring Type::getType() const
{
    return type;
}

std::string Type::translateTypeToCpp() const
{
    return Type::typeMap.at(type);
}
