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
    explicit IFuncDeclStmt(const Token& token, ClassDeclStmt* classDecl = nullptr) : Stmt(token, classDecl) {}
    ~IFuncDeclStmt() override = default;

    virtual std::string getName() const = 0;
    virtual const std::vector<std::unique_ptr<FuncCreditStmt>>& getCredited() const {return emptyCredits;}
    virtual std::unique_ptr<IType> getReturnType() const = 0;
    virtual void setHasReturned(const bool hasReturned) {}
};
