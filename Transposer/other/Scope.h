#pragma once
#include <memory>
#include <vector>

#include "Var.h"
#include "token/Token.h"

class Scope
{
private:
    std::vector<Var> vars;
    Scope* parent;
    std::vector<std::unique_ptr<Scope>> children;

public:
    Scope(Scope* parent = nullptr);
    bool doesVarExist(const Var& v) const;

    Scope* makeNewScope();
    Scope* getParent() const;
    void addVar(const Type& type, const Token& token);
};
