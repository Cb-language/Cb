#pragma once
#include "AST/abstract/Expression.h"

class CastExpr : public Expr
{
private:
    std::unique_ptr<Expr> expr;
    std::unique_ptr<IType> type;

public:
    CastExpr(const Token& token, std::unique_ptr<Expr> expr, std::unique_ptr<IType> type);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
};
