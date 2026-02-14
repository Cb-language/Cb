#pragma once
#include "AST/abstract/Statement.h"

class BreakStmt : public Stmt
{
private:

public:
    BreakStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass);
    void analyze() const override;
    std::string translateToCpp() const override;
};
