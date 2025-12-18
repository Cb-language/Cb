#pragma once
#include "AST/abstract/Statement.h"

class BreakStmt : public Stmt
{
private:

public:
    BreakStmt(Scope* scope, FuncDeclStmt* funcDecl);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
