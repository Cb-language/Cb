#pragma once
#include "AST/abstract/Call.h"

class ArraySlicingExpr : public Call
{
private:
    std::unique_ptr<Call> call;
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> stop;
    std::unique_ptr<Expr> step;
public:
    ArraySlicingExpr(const Token& token,
        std::unique_ptr<Call> call);

    ArraySlicingExpr(const ArraySlicingExpr& other);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::string toString() const override;

    void setStart(std::unique_ptr<Expr> start);
    void setStop(std::unique_ptr<Expr> stop);
    void setStep(std::unique_ptr<Expr> step);
};
