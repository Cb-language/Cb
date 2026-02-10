#pragma once
#include "AST/abstract/Statement.h"

class ContinueStmt : public Stmt
{
private:

public:
    ContinueStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl);
    void analyze() const override;
    std::string translateToCpp() const override;
};
