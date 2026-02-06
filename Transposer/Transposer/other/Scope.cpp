#include "Scope.h"

#include <ranges>

#include "Utils.h"
#include "../errorHandling/semanticErrors/IdentifierTaken.h"
#include "../errorHandling/syntaxErrors/UnexpectedToken.h"

Scope::Scope(Scope* parent) : parent(parent)
{
}

Scope::Scope(Scope* parent, const bool isBreakable, const bool isContinueAble) : parent(parent), isBreakable(isBreakable), isContinueAble(isContinueAble)
{
}

Scope::~Scope()
{
    parent = nullptr;
    vars.clear();

    for (auto& kid : children)
    {
        kid.reset();
        kid = nullptr;
    }

    children.clear();
}

std::optional<Var> Scope::getVar(const std::wstring& name) const
{
    for (const auto& var : vars | std::views::keys)
    {
        if (name == var.getName())
        {
            return var.copy();
        }
    }

    if (parent != nullptr)
    {
        return parent->getVar(name);
    }

    return std::nullopt;
}

Scope* Scope::makeNewScope(bool isBreakable, bool isContinueAble)
{
    auto s = std::make_unique<Scope>(this, isBreakable, isContinueAble);
    Scope* raw = s.get();       // keep raw pointer
    children.push_back(std::move(s));
    return raw;
}

Scope* Scope::getParent() const
{
    return parent;
}

void Scope::addVar(std::unique_ptr<IType> type, const Token& token)
{
    if (token.type != Token::IDENTIFIER)
    {
        throw UnexpectedToken(token);
    }

    Var v = Var(std::move(type) , token.value);
    for (const auto& var : vars | std::views::keys)
    {
        if (v == var)
        {
            throw IdentifierTaken(token);
        }
    }

    vars.emplace_back(std::move(v), token);
}

void Scope::addVar(const Var& var, const Token& token)
{
    for (const auto& v : vars| std::views::keys)
    {
        if (var == v)
        {
            throw IdentifierTaken(token);
        }
    }

    vars.emplace_back(var.copy(), token);
}

int Scope::getLevel() const
{
    int level = 0;

    const Scope* current = parent;

    while (current != nullptr)
    {
        ++level;
        current = current->parent;
    }

    return level;
}

bool Scope::getIsBreakable() const
{
    if (isBreakable)
    {
        return true;
    }

    if (getParent() != nullptr)
    {
        return getParent()->getIsBreakable();
    }
    return false;
}

bool Scope::getIsContinueAble() const
{
    if (isContinueAble)
    {
        return true;
    }

    if (getParent() != nullptr)
    {
        return getParent()->getIsContinueAble();
    }
    return false;
}

const std::optional<Class>& Scope::getCurrClass() const
{
    if (!currClass.has_value())
    {
        if (parent == nullptr) return std::nullopt;
        return parent->getCurrClass();
    }

    return currClass;
}

const std::wstring& Scope::getCurrClassName() const
{
    auto c = getCurrClass();
    if (c.has_value()) return c->getClassName();
    return L"";
}

void Scope::setCurrClass(Class& currClass)
{
    this->currClass.emplace(currClass.copy());
}

const std::vector<std::pair<Var, Token>>& Scope::getCurrVars() const
{
    return vars;
}
