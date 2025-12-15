#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "../symbols/Var.h"
#include "token/Token.h"

class Scope
{
private:
    std::vector<Var> vars;
    Scope* parent;
    std::vector<std::unique_ptr<Scope>> children;

public:
    explicit Scope(Scope* parent = nullptr);
    ~Scope();

    // std::nullopt when not found
    std::optional<Var> getVar(const std::wstring& name) const;

    Scope* makeNewScope();
    Scope* getParent() const;
    void addVar(std::unique_ptr<IType> type, const Token& token);
    void addVar(const Var& var, const Token& token);
    int getLevel() const;
};
