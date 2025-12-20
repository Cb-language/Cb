#pragma once
#include "VarDeclStmt.h"

class ArrayDeclStmt : public VarDeclStmt
{
    std::vector<std::unique_ptr<Expr>> sizes;

    bool isLegalSizes() const;
    std::string createConstructor(IType* type, const size_t dim) const;
public:
    ArrayDeclStmt(Scope* scope, FuncDeclStmt* funcDecl, bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var &var, std::vector<std::unique_ptr<Expr>> sizes);

    bool isLegal() const override;
    std::string translateToCpp() const override;
};

