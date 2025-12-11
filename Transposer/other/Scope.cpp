#include "Scope.h"

#include "errorHandling/lexicalErrors/IdentifierTaken.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

Scope::Scope(Scope* parent) : parent(parent)
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
    for (const auto& var : vars)
    {
        if (name == var.getName())
        {
            return var;
        }
    }

    if (parent != nullptr)
    {
        return parent->getVar(name);
    }

    return std::nullopt;
}

Scope* Scope::makeNewScope()
{
    auto s = std::make_unique<Scope>(this);
    Scope* raw = s.get();       // keep raw pointer
    children.push_back(std::move(s));
    return raw;
}

Scope* Scope::getParent() const
{
    return parent;
}

void Scope::addVar(const Type& type, const Token& token)
{
    if (token.type != Token::IDENTIFIER)
    {
        throw UnexpectedToken(token);
    }

    const Var v = Var(type , token.value);
    for (const auto& var : vars)
    {
        if (v == var)
        {
            throw IdentifierTaken(token);
        }
    }

    vars.push_back(v);
}

void Scope::addVar(const Var& var, const Token& token)
{
    for (const auto& v : vars)
    {
        if (var == v)
        {
            throw IdentifierTaken(token);
        }
    }

    vars.push_back(var);
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
