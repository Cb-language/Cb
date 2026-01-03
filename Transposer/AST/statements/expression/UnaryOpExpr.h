#pragma once
#include "AST/abstract/Call.h"
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
    const std::unique_ptr<Call> call;
    const UnaryOp op;
    const bool isStmt;
public:
    UnaryOpExpr(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Call> call, const UnaryOp op, const bool isStmt = false);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
};