#pragma once
#include "AST/abstract/Statement.h"
#include "symbols/FuncCredit.h"

class FuncCreditStmt : public Stmt
{
private:
    const FuncCredit funcCredit;

public:
    FuncCreditStmt(Scope* scope, FuncDeclStmt* funcDecl,const FuncCredit& funcCredit);

    bool isLegal() const override;
    std::string translateToCpp() const override;
};
