#pragma once
#include <memory>
#include <optional>

#include "AST/abstract/Expression.h"
#include "../../../symbols/Var.h"

class BinaryOpExpr : public Expr
{
private:
    const std::wstring op;
    const std::unique_ptr<Expr> left;
    const std::unique_ptr<Expr> right;

public:
    BinaryOpExpr(Scope* scope, FuncDeclStmt* funcDecl, const std::wstring& op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, const bool hasParens = false);

    std::unique_ptr<IType> getType() const override;
    bool isLegal() const override;
    std::string translateToCpp() const override;

    static int getPrecedence(const std::wstring &op) ;
};
