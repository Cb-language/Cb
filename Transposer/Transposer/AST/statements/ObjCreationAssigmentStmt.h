#pragma once
#include "AST/abstract/ObjectCreationStmt.h"

class ObjCreationAssigmentStmt : public ObjectCreationStmt
{
public:
    ObjCreationAssigmentStmt(const Token& token, std::unique_ptr<Expr> startingValue, const Var &var);
    std::string translateToCpp() const override;
};
