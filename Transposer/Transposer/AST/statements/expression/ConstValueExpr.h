#pragma once

#include "AST/abstract/Expression.h"

class ConstValueExpr : public Expr
{
private:
    std::unique_ptr<IType> type;
    std::wstring value;
public:
    ConstValueExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, std::unique_ptr<IType> type, const std::wstring &value);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::wstring getValue() const;
};