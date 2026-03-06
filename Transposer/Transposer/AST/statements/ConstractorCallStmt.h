#pragma once
#include "AST/abstract/Expression.h"

class ConstractorCallStmt : public Expr
{
private:
    const ClassNode* classNode;
    std::vector<std::unique_ptr<Expr>> args;
    bool isStmt = false;

public:
    ConstractorCallStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
        const ClassNode* classNode, std::vector<std::unique_ptr<Expr>> args);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;

    void setIsStmt(const bool isStmt) override;

    const ClassNode* getClassNode() const;
};
