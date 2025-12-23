#pragma once
#include "AST/abstract/Statement.h"

class ContinueStmt : public Stmt
{
private:

public:
    ContinueStmt(Scope* scope, FuncDeclStmt* funcDecl);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
