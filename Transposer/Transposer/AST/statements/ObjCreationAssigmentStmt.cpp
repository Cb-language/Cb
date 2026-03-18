#include "ObjCreationAssigmentStmt.h"

ObjCreationAssigmentStmt::ObjCreationAssigmentStmt(const Token& token, std::unique_ptr<Expr> startingValue,
    const Var& var) : ObjectCreationStmt(token, true, std::move(startingValue), var)
{
}

std::string ObjCreationAssigmentStmt::translateToCpp() const
{
    return ObjectCreationStmt::translateToCpp() + "\n";
}

