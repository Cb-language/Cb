#pragma once
#include <vector>

#include "VarDeclStmt.h"

class ArrayDeclStmt : public VarDeclStmt
{
    std::vector<std::unique_ptr<Expr>> sizes;

    void analyzeSizes() const;
    std::string createConstructor(IType* type, const size_t dim) const;
public:
    ArrayDeclStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var &var, std::vector<std::unique_ptr<Expr>> sizes);

    void analyze() const override;
    std::string translateToCpp() const override;
};

