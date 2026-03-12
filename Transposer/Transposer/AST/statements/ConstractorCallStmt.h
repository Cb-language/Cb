#pragma once
#include "AST/abstract/Expression.h"

class ConstractorCallStmt : public Expr
{
private:
    std::vector<std::unique_ptr<Expr>> args;
    bool needsSemicolon = false;
    const ClassNode* targetClass = nullptr;

public:
    ConstractorCallStmt(const Token& token, std::vector<std::unique_ptr<Expr>> args);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;

    void setNeedsSemicolon(const bool needsSemicolon) override;

    const ClassNode* getClassNode() const;
    void setTargetClass(const ClassNode* targetClass);
};
