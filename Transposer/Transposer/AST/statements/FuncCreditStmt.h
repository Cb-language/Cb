#pragma once
#include "AST/abstract/Statement.h"
#include "symbols/FuncCredit.h"

class FuncCreditStmt : public Stmt
{
private:
    const FuncCredit funcCredit;

public:
    FuncCreditStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl,const FuncCredit& funcCredit);

    const std::wstring& getName() const;

    void analyze() const override;
    std::string translateToCpp() const override;
};
