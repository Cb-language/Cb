#pragma once
#include "BinaryOpExpr.h"
#include "AST/abstract/Call.h"

class DotOpExpr : public Call
{
private:
    std::unique_ptr<Call> left;
    std::unique_ptr<Call> right;

public:
    DotOpExpr(const Token& token,
        std::unique_ptr<Call> left, std::unique_ptr<Call> right);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string toString() const override;
};
