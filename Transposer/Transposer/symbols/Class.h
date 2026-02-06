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

    const std::wstring& getClassName() const;
    const std::vector<std::pair<bool, Func>>& getMethods() const;
    const std::vector<std::pair<bool, Var>>& getFields() const;
    const std::vector<std::pair<bool, Constractor>>& getConstractors() const;
};
