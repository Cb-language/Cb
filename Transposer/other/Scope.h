#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "../symbols/Var.h"
#include "token/Token.h"

class Scope
{
private:
    std::vector<std::pair<Var, Token>> vars;
    Scope* parent;
    std::vector<std::unique_ptr<Scope>> children;
    const bool isBreakable = false;
    const bool isContinueAble = false;
public:
    explicit Scope(Scope* parent = nullptr);
    explicit Scope(Scope* parent = nullptr, bool isBreakable = false, bool isContinueAble = false);
    ~Scope();

    // std::nullopt when not found
    std::optional<Var> getVar(const std::wstring& name) const;

    Scope* makeNewScope(bool isBreakable, bool isContinueAble);
    Scope* getParent() const;
    void addVar(std::unique_ptr<IType> type, const Token& token);
    void addVar(const Var& var, const Token& token);
    int getLevel() const;
    bool getIsBreakable() const;
    bool getIsContinueAble() const;

    const std::vector<std::pair<Var, Token>>& getCurrVars() const;
};
