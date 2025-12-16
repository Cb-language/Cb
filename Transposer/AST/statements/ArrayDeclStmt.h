#pragma once
#include "VarDeclStmt.h"

class ArrayDeclStmt : public VarDeclStmt
{
    std::unique_ptr<Expr> size;
public:
    ArrayDeclStmt(Scope* scope, FuncDeclStmt* funcDecl, bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var &var, std::unique_ptr<Expr> size);

    bool isLegal() const override;
    std::string translateToCpp() const override;
};

