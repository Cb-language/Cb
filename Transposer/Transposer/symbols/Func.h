#pragma once
#include <vector>

#include "Type/Type.h"
#include "Var.h"
#include "AST/abstract/VirtualType.h"

class ClassNode;

class Func
{
private:
    std::unique_ptr<IType> rType;
    const std::string funcName;
    std::vector<Var> args;
    VirtualType virtualType;
    const ClassNode* owner;
    bool isStatic;

public:
    Func(std::unique_ptr<IType> rType, const std::string& funcName, const std::vector<Var>& args, const VirtualType vType, const ClassNode* owner = nullptr, bool isStatic = false);
    Func(const Func& other);

    const std::vector<Var>& getArgs() const;
    const std::string& getFuncName() const;
    std::unique_ptr<IType> getType() const;

    void setVirtual(const VirtualType vType);
    VirtualType getVirtual() const;

    void setOwner(const ClassNode* owner);
    const ClassNode* getOwner() const;

    bool getStatic() const;
    void setStatic(bool isStatic);

    std::string translateToCpp(const std::string& className = "") const;

    bool operator==(const Func& other) const;
    bool operator!=(const Func& other) const;

    bool isSameNameAndArgs(const Func& other) const;

    Func copy() const;

    bool operator<(const Func& other) const;
};
