#pragma once

#include "AST/abstract/Expression.h"

class ConstValueExpr : public Expr
{
private:
    Type type;
    std::wstring value;
public:
    ConstValueExpr(Scope* scope, FuncDeclStmt* funcDecl, const Type &type, const std::wstring &value);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    Type getType() const override;
    std::wstring getValue() const;
};