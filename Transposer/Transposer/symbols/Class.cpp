#include "Class.h"

Class::Class(const std::wstring& name, const std::vector<std::pair<bool, Func>>& methods,
             const std::vector<std::pair<bool, Var>>& fields, const std::vector<std::pair<bool, Constractor>>& constractors) : name(name)
{
    for (const auto& [isPublic, method] : methods) this->methods.emplace_back(isPublic, method.copy());
    for (const auto& [isPublic, field] : fields) this->fields.emplace_back(isPublic, field.copy());
    for (const auto& [isPublic, constractor] : constractors) this->constractors.emplace_back(isPublic, constractor.copy());
}

Class::Class(const Class& other) : Class(other.name, other.methods, other.fields, other.constractors)
{
}

Class::Class(const std::wstring& name) : name(name)
{
}

const std::wstring& Class::getClassName() const
{
    return name;
}

const std::vector<std::pair<bool, Func>>& Class::getMethods() const
{
    return methods;
}

const std::vector<std::pair<bool, Var>>& Class::getFields() const
{
    return fields;
}

const std::vector<std::pair<bool, Constractor>>& Class::getConstractors() const
{
    return constractors;
}

void Class::addMethod(const bool isPublic, const Func& method)
{
    methods.emplace_back(isPublic, method.copy());
}

void Class::addField(const bool isPublic, const Var& field)
{
    fields.emplace_back(isPublic, field.copy());
}

void Class::addConstractor(const bool isPublic, const Constractor& constractor)
{
    constractors.emplace_back(isPublic, constractor.copy());
}

Class Class::copy() const
{
    return Class(*this);
}
