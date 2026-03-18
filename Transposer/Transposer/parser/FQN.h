#pragma once
#include <string>
#include <vector>

using FQN = std::vector<std::string>; // FQN = fully qualified name (name that is seperated with \)

inline std::string translateFQNtoString(const FQN& stmts)
{
    std::string ret = "";
    bool first = true;

    for (const auto& stmt : stmts)
    {
        if (!first)
        {
            ret += "->";
        }
        ret += stmt;
        first = false;
    }
    return ret;
}