#pragma once

#include <locale>
#include <codecvt>

struct Stmt
{
public:
    virtual ~Stmt() = default;
    virtual bool isLegal() const = 0;
    virtual std::string translateToCpp() const = 0;
};