#pragma once
#include "AST/abstract/Statement.h"

class BreakStmt : public Stmt
{
private:

public:
    BreakStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl);
    void analyze() const override;
    std::string translateToCpp() const override;
};
