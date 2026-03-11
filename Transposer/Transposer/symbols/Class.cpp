#include "Class.h"

#include <ranges>

Class::Class(const FQN& name, const std::vector<std::pair<AccessType, Func>>& methods,
    const std::vector<std::pair<AccessType, Var>>& fields,
    const std::vector<std::pair<AccessType, Constractor>>& constractors, bool isAbstract)
    : name(name), _isAbstract(isAbstract)
{
    for (const auto& [accessType, method] : methods) this->methods.emplace_back(accessType, method.copy());
    for (const auto& [accessType, field] : fields) this->fields.emplace_back(accessType, field.copy());
    for (const auto& [accessType, constractor] : constractors) this->constractors.emplace_back(accessType, constractor.copy());
}

Class::Class(const Class& other) : Class(other.name, other.methods, other.fields, other.constractors, other._isAbstract)
{
}

Class::Class(const FQN& name) : name(name), _isAbstract(false)
{
}

const FQN& Class::getClassName() const
{
    return name;
}

const std::vector<std::pair<AccessType, Func>>& Class::getMethods() const
{
    return methods;
}

const std::vector<std::pair<AccessType, Var>>& Class::getFields() const
{
    return fields;
}

const std::vector<std::pair<AccessType, Constractor>>& Class::getConstractors() const
{
    return constractors;
}

void Class::setAbstract(const bool isAbstract)
{
    this->_isAbstract = isAbstract;
}

bool Class::isAbstract() const
{
    return _isAbstract;
}

void Class::addMethod(const AccessType accessType, const Func& method)
{
    methods.emplace_back(accessType, method.copy());
}

void Class::addField(const AccessType accessType, const Var& field)
{
    fields.emplace_back(accessType, field.copy());
}

void Class::addConstractor(const AccessType accessType, const Constractor& constractor)
{
    constractors.emplace_back(accessType, constractor.copy());
}

bool Class::hasMethod(const Func& method) const
{
    for (const auto& m : methods | std::views::values)
    {
        if (m == method) return true;
    }
    return false;
}

bool Class::hasField(const Var& field) const
{
    for (const auto& f : fields | std::views::values)
    {
        if (f == field) return true;
    }
    return false;
}

bool Class::hasConstractor(const Constractor& constractor) const
{
    for (const auto& c : constractors | std::views::values)
    {
        if (c == constractor) return true;
    }
    return false;
}

bool Class::hasMethod(FQN& name) const
{
    for (const auto& m : methods | std::views::values)
    {
        if (translateFQNtoString(m.getFuncName()) == translateFQNtoString(name)) return true;
    }
    return false;
}

bool Class::hasField(const FQN& name) const
{
    for (const auto& f : fields | std::views::values)
    {
        if (translateFQNtoString(f.getName()) == translateFQNtoString(name)) return true;
    }
    return false;
}

Class Class::copy() const
{
    return Class(*this);
}
