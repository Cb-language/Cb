#pragma once
#include <vector>

#include "Type/Type.h"
#include "Var.h"
#include "AST/abstract/VirtualType.h"

class Func
{
private:
    std::unique_ptr<IType> rType;
    const std::wstring funcName;
    std::vector<Var> args;
    VirtualType virtualType;

public:
    Func(std::unique_ptr<IType> rType, const std::wstring& funcName, const std::vector<Var>& args, const VirtualType vType);
    Func(const Func& other);

    const std::vector<Var>& getArgs() const;
    const std::wstring& getFuncName() const;
    std::unique_ptr<IType> getType() const;
    void setVirtual(const VirtualType vType);
    VirtualType getVirtual() const;

    std::string translateToCpp(const std::wstring& className = L"") const;

    bool operator==(const Func& other) const;
    bool operator!=(const Func& other) const;

    bool isSameNameAndArgs(const Func& other) const;

    Func copy() const;

    bool operator<(const Func& other) const;
};
