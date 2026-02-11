#pragma once
#include <vector>

#include "Constractor.h"
#include "Func.h"

class Class
{
private:
    const std::wstring name;
    std::vector<std::pair<bool, Func>> methods;
    std::vector<std::pair<bool, Var>> fields;
    std::vector<std::pair<bool, Constractor>> constractors;

public:
    Class(const std::wstring& name, const std::vector<std::pair<bool, Func>>& methods,
        const std::vector<std::pair<bool, Var>>& fields, const std::vector<std::pair<bool, Constractor>>& constractors);

    Class(const Class& other);

    explicit Class(const std::wstring& name);

    const std::wstring& getClassName() const;
    const std::vector<std::pair<bool, Func>>& getMethods() const;
    const std::vector<std::pair<bool, Var>>& getFields() const;
    const std::vector<std::pair<bool, Constractor>>& getConstractors() const;

    void addMethod(const bool isPublic, const Func& method);
    void addField(const bool isPublic, const Var& field);
    void addConstractor(const bool isPublic, const Constractor& constractor);

    bool hasMethod(const Func& method) const;
    bool hasField(const Var& field) const;
    bool hasConstractor(const Constractor& constractor) const;

    bool hasMethod(const std::wstring& name);
    bool hasField(const std::wstring& name);

    Class copy() const;
};
