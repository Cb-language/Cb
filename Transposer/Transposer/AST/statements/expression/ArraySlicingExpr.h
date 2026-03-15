#pragma once
#include "AST/abstract/VarReference.h"

class ArraySlicingExpr : public VarReference
{
private:
    std::unique_ptr<VarReference> varRef;
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> stop;
    std::unique_ptr<Expr> step;
public:
    ArraySlicingExpr(const Token& token,
        std::unique_ptr<VarReference> ref);

    ArraySlicingExpr(const ArraySlicingExpr& other);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::string toString() const override;
    void setSymbolTable(SymbolTable* symTable) const override;

    void setStart(std::unique_ptr<Expr> start);
    void setStop(std::unique_ptr<Expr> stop);
    void setStep(std::unique_ptr<Expr> step);
};
