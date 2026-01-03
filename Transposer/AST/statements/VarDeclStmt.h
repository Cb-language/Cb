#pragma once

#include <memory>
#include <optional>


#include "../../symbols/Var.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class VarDeclStmt : public Stmt
{
protected:
    const bool hasStartingValue;
    const std::unique_ptr<Expr> startingValue;
    const Var var;

public:
    VarDeclStmt(Scope* scope, FuncDeclStmt* funcDecl, bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var &var);

    void analyze() const override;
    std::string translateToCpp() const override;
};
