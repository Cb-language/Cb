#pragma once
#include "AST/abstract/Call.h"

class ArrayIndexingExpr : public Call
{
private:
    std::unique_ptr<Call> call;
    std::unique_ptr<Expr> index;

public:
    ArrayIndexingExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, std::unique_ptr<Call> call, std::unique_ptr<Expr> index);

    std::unique_ptr<IType> getType() const override;
    std::string translateToCpp() const override;
    void analyze() const override;
};
