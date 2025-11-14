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
    {L"bar",     {L"bar"}},                          // std::string → only string
    {L"note",    {L"note", L"flat", L"freq"}},      // char → int, double
    {L"mute",    {L"mute", L"flat", L"freq"}},      // bool → int, double
    {L"sharp",   {L"sharp", L"flat", L"freq"}},     // unsigned → int, double
    {L"flat",    {L"flat", L"freq"}},               // int → double
    {L"degree",  {L"degree", L"flat", L"freq"}},    // int → double
    {L"freq",    {L"freq"}},                        // double → double
//  {L"scale",   {L"scale"}},                      // Array - TODO: add this in sprint 3
    {L"fermata", {}}                                // void → nothing
};

Type::Type(std::wstring  type) : type(std::move(type))
{
}

bool Type::operator==(const Type& other) const
{
    const auto it = castMap.find(other.type);
    if (it == castMap.end() && type != other.type) return false; // the second is for future (class)
    return it->second.contains(other.type);
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
