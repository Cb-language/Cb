#pragma once

#include "BodyStmt.h"
#include "FuncCreditStmt.h"
#include "AST/abstract/IFuncDeclStmt.h"
#include "AST/abstract/Statement.h"
#include "AST/abstract/VirtualType.h"

// DO NOT DELETE - CIRCULAR INCLUDES
class FuncCreditStmt;
class BodyStmt;

class FuncDeclStmt : public IFuncDeclStmt
{
private:
    Func func;
    std::vector<std::unique_ptr<FuncCreditStmt>> credited;
    std::unique_ptr<BodyStmt> body;
    bool hasReturned;
    VirtualType virtualType = VirtualType::None;
    bool isMethod;

public:
    FuncDeclStmt(const Token& token, Scope* scope, const ClassNode* currClass,
        const std::wstring &funcName, std::unique_ptr<IType> returnType, const std::vector<Var> &args, std::vector<std::unique_ptr<FuncCreditStmt>>& credited, bool isMethod, const VirtualType& virtualType);

    const std::vector<Var>& getArgs() const;
    std::wstring getName() const override;
    std::unique_ptr<IType> getReturnType() const override;
    const Func& getFunc() const;
    void setVirtual(const VirtualType vType);
    VirtualType getVirtual() const;

    // is called after the ctor
    void setBody(std::unique_ptr<BodyStmt> body);

    void setHasReturned(const bool hasReturned) override;
    bool getHasReturned() const;

    const std::vector<std::unique_ptr<FuncCreditStmt>>& getCredited() const override;

    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;
    std::string translateToCppClass(const std::wstring& className) const;
};
