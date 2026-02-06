#include "Class.h"

Class::Class(const std::wstring& name, const std::vector<std::pair<bool, Func>>& methods,
    const std::vector<std::pair<bool, Var>>& fields, const std::vector<std::pair<bool, Constractor>>& constractors) : name(name)
{
    for (const auto& [isPublic, method] : methods) this->methods.emplace_back(isPublic, method.copy());
    for (const auto& [isPublic, field] : fields) this->fields.emplace_back(isPublic, field.copy());
    for (const auto& [isPublic, constractor] : constractors) this->constractors.emplace_back(isPublic, constractor.copy());
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
