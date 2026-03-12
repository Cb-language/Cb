#pragma once
#include "BodyStmt.h"
#include "AST/abstract/IFuncDeclStmt.h"
#include "AST/abstract/Statement.h"
#include "ConstractorCallStmt.h"
#include "symbols/Constractor.h"

class ConstructorDeclStmt : public IFuncDeclStmt
{
private:
    std::unique_ptr<BodyStmt> body;
    Constractor constractor;
    std::unique_ptr<ConstractorCallStmt> parentCtorCall;

public:
    ConstructorDeclStmt(const Token& token, const FQN& className, const std::vector<Var>& args);

    void setParentCtorCall(std::vector<std::unique_ptr<Expr>> args);

    void setBody(std::unique_ptr<BodyStmt> body);

    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;

    const FQN& getName() const override;
    std::unique_ptr<IType> getReturnType() const override;

    const Constractor& getConstractor() const;
};
