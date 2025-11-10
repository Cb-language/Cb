#pragma once

#include <iostream>

struct Stmt
{
public:
    virtual ~Stmt() = default;
    virtual bool isLegal() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Stmt& stmt);
};