#pragma once
#include "BodyStmt.h"
#include "AST/abstract/IFuncDeclStmt.h"
#include "AST/abstract/Statement.h"
#include "ConstractorCallStmt.h"
#include "symbols/Constractor.h"

class ConstractorDeclStmt : public IFuncDeclStmt
{
private:
    std::unique_ptr<BodyStmt> body;
    Constractor constractor;
    std::unique_ptr<ConstractorCallStmt> parentCtorCall;

public:
    ConstractorDeclStmt(const Token& token, Scope* scope, const ClassNode* currClass, const std::wstring& className, const std::vector<Var>& args);

    void setParentCtorCall(std::vector<std::unique_ptr<Expr>> args);

    void setBody(std::unique_ptr<BodyStmt> body);

    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;

    std::wstring getName() const override;
    std::unique_ptr<IType> getReturnType() const override;

    const Constractor& getConstractor() const;
};
