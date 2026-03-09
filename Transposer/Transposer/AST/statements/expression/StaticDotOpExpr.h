#pragma once
#include "AST/abstract/Call.h"
#include "symbols/Type/ClassType.h"

class StaticDotOpExpr : public Call
{
private:
    std::unique_ptr<ClassType> left;
    std::unique_ptr<Call> right;

public:
    StaticDotOpExpr(const Token& token, IFuncDeclStmt* funcDecl, std::unique_ptr<ClassType> left, std::unique_ptr<Call> right, const bool needsSemicolon = false, ClassDeclStmt* classDecl = nullptr);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string toString() const override;
};
