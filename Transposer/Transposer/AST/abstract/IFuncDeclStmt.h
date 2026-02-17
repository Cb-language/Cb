#pragma once
#include <memory>
#include <vector>

#include "Statement.h"
#include "../statements/FuncCreditStmt.h"

class IFuncDeclStmt : public Stmt
{
private:
    // ReSharper disable once CppVariableCanBeMadeConstexpr
    // ReSharper disable once CppUseAuto
    inline static const std::vector<std::unique_ptr<FuncCreditStmt>> emptyCredits = std::vector<std::unique_ptr<FuncCreditStmt>>();
public:
    IFuncDeclStmt(const Token& token, Scope* scope, const ClassNode* currClass) : Stmt(token, scope, currClass) {}
    ~IFuncDeclStmt() override = default;

    virtual std::wstring getName() const = 0;
    virtual const std::vector<std::unique_ptr<FuncCreditStmt>>& getCredited() const {return emptyCredits;}
    virtual std::unique_ptr<IType> getReturnType() const = 0;
    virtual void setHasReturned(const bool hasReturned) {}
};
