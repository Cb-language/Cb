#pragma once
#include "AST/abstract/Statement.h"
#include "symbols/FuncCredit.h"

class FuncCreditStmt : public Stmt
{
private:
    const FuncCredit funcCredit;

public:
    FuncCreditStmt(const Token& token, IFuncDeclStmt* funcDecl, const FuncCredit& funcCredit, ClassDeclStmt* classDecl = nullptr);

    std::string getName() const;

    void analyze() const override;
    std::string translateToCpp() const override;
};
