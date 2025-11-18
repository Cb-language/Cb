#include "Scope.h"

#include "errorHandling/lexicalErrors/IdentifierTaken.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

Scope::Scope(Scope* parent) : parent(parent)
{
}

bool Scope::doesVarExist(const Var& v) const
{
    for (const auto& var : vars)
    {
        if (v == var)
        {
            return true;
        }
    }

    if (parent != nullptr)
    {
        return parent->doesVarExist(v);
    }

    return false;
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
