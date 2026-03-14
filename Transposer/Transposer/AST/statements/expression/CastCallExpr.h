#pragma once
#include "AST/abstract/Call.h"

class CastCallExpr : public Call
{
private:
    std::unique_ptr<Expr> expr;
    std::unique_ptr<IType> type;

public:
    CastCallExpr(const Token& token, std::unique_ptr<Expr> expr, std::unique_ptr<IType> type);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string toString() const override;
};
