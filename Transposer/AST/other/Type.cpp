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
    {L"bar",     {L"bar"}},                               // std::string -> only string
    {L"note",    {L"note", L"flat", L"freq"}},      // char -> int, double
    {L"mute",    {L"mute", L"flat", L"freq"}},      // bool -> int, double
    {L"degree",  {L"degree", L"flat", L"freq"}},    // int -> char, double
    {L"freq",    {L"degree", L"flat", L"freq"}},    // double -> int, char
//  {L"scale",   {L"scale"}},                                             // Array - TODO: add this in sprint 3
    {L"fermata", {}}                                         // void -> nothing
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

std::ostream& operator<<(std::ostream& os, const Type& t)
{
    os << Type::typeMap.at(t.type);
    return os;
}
