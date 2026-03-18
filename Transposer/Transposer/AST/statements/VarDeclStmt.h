#pragma once

#include <memory>

#include "../../symbols/Var.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class VarDeclStmt : public Expr
{
protected:
    const bool hasStartingValue;
    const std::unique_ptr<Expr> startingValue;
    Var var;

public:
    VarDeclStmt(const Token& token,
        bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var &var);

    void analyze() const override;
    std::string translateToCpp() const override;
    void setSymbolTable(SymbolTable* symTable) const override;

    const Var& getVar() const;
    const Expr* getStartingValue() const;
    void setIsStatic(const bool isStatic);
    std::unique_ptr<IType> getType() const override;
};
