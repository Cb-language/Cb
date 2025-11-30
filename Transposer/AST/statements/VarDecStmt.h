#pragma once

#include <memory>
#include <optional>

#include "../Statements.h"
#include "../../symbols/Var.h"

class VarDecStmt : public Stmt
{
private:
    const bool hasStartingValue;
    const std::unique_ptr<Expr> startingValue;
    const Var var;

public:
    VarDecStmt(Scope* scope, FuncDeclStmt* funcDecl, bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var &var);

    bool isLegal() const override;
    std::string translateToCpp() const override;
};
