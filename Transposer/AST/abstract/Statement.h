#pragma once

#include <locale>
#include "other/Utils.h"

struct Stmt
{
public:
    virtual ~Stmt() = default;
    virtual bool isLegal() const = 0;
    virtual std::string translateToCpp() const = 0;
};