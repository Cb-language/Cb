#pragma once
#include "VarDeclStmt.h"
#include "AST/abstract/ObjectCreationStmt.h"
#include "expression/CastCallExpr.h"

class ObjCreationCastStmt : public ObjectCreationStmt
{
public:
    ObjCreationCastStmt(const Token& token, bool hasStartingValue, std::unique_ptr<CastCallExpr> startingValue, const Var& var);

    void analyze() const override;
    std::string translateToCpp() const override;
};
