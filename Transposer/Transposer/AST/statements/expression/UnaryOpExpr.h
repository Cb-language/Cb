#pragma once
#include "AST/abstract/Call.h"
#include "AST/abstract/Expression.h"

enum class UnaryOp
{
    Zero = 0,
    PlusPlus,
    MinusMinus,
    PlusPlusPlusPlus,
    MinusMinusMinusMinus,
    Not
};

class UnaryOpExpr : public Expr
{
private:
    const std::unique_ptr<Call> call;
    const UnaryOp op;
    const bool needsSemicolon;
public:
    UnaryOpExpr(const Token& token, IFuncDeclStmt* funcDecl,
        std::unique_ptr<Call> call, const UnaryOp op, const bool needsSemicolon = false, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
};