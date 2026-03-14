#pragma once

#include "AST/abstract/Expression.h"

class ConstValueExpr : public Expr
{
private:
    std::unique_ptr<IType> type;
    std::string value;

    std::string getValueStr() const;
public:
    ConstValueExpr(const Token& token, std::unique_ptr<IType> type, const std::string &value);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::string getValue() const;
};