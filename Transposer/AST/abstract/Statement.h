#pragma once

#include <iostream>

struct Stmt
{
public:
    virtual ~Stmt() = default;
    virtual bool isLegal() const = 0;
    virtual std::string print() const = 0;
};