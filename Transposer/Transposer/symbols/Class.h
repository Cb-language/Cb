#pragma once
#include <vector>

#include "Constractor.h"
#include "Func.h"
#include "class/AccessType.h"

class Class
{
private:
    FQN name;
    std::vector<std::pair<AccessType, Func>> methods;
    std::vector<std::pair<AccessType, Var>> fields;
    std::vector<std::pair<AccessType, Constractor>> constractors;
    bool _isAbstract;

public:
    Class(const FQN& name, const std::vector<std::pair<AccessType, Func>>& methods,
        const std::vector<std::pair<AccessType, Var>>& fields, const std::vector<std::pair<AccessType, Constractor>>& constractors, bool isAbstract = false);

    Class(const Class& other);

    explicit Class(const FQN& name);

    const FQN& getClassName() const;
    const std::vector<std::pair<AccessType, Func>>& getMethods() const;
    const std::vector<std::pair<AccessType, Var>>& getFields() const;
    const std::vector<std::pair<AccessType, Constractor>>& getConstractors() const;
    void setAbstract(const bool isAbstract);
    bool isAbstract() const;

    void addMethod(const AccessType accessType, const Func& method);
    void addField(const AccessType accessType, const Var& field);
    void addConstractor(const AccessType accessType, const Constractor& constractor);

    bool hasMethod(const Func& method) const;
    bool hasField(const Var& field) const;
    bool hasConstractor(const Constractor& constractor) const;

    bool hasMethod(FQN& name) const;
    bool hasField(const FQN& name) const;

    Class copy() const;
};
