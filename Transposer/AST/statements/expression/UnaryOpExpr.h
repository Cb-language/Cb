#pragma once
#include "VarCallExpr.h"
#include "AST/abstract/Expression.h"

enum class UnaryOp
{
    Zero = 0,
    PlusPlus,
    MinusMinus,
    Not
};

class UnaryOpExpr : public Expr
{
private:
    const std::unique_ptr<VarCallExpr> var;
    const UnaryOp op;
    const bool isStmt;
public:
    UnaryOpExpr(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<VarCallExpr>, const UnaryOp op, const bool isStmt = false);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
};