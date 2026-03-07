#pragma once
#include "AST/abstract/Call.h"
#include "AST/abstract/Expression.h"
#include "symbols/Type/ClassType.h"

class StaticDotOpExpr : public Expr
{
private:
    std::unique_ptr<ClassType> left;
    std::unique_ptr<Call> right;

public:
    StaticDotOpExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, std::unique_ptr<ClassType> left, std::unique_ptr<Call> right, const bool isStmt = false);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
};
