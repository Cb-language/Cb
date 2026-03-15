#pragma once
#include "AST/statements/VarDeclStmt.h"

class ObjectCreationStmt : public VarDeclStmt
{
public:
    ObjectCreationStmt(const Token& token, bool hasStartingValue, std::unique_ptr<Expr> startingValue,
        const Var& var);

    void analyze() const override = 0;
    std::string translateToCpp() const override = 0;
};

inline ObjectCreationStmt::ObjectCreationStmt(const Token& token, const bool hasStartingValue, std::unique_ptr<Expr> startingValue,
        const Var& var)
        : VarDeclStmt(token, hasStartingValue, std::move(startingValue), var)
{}
