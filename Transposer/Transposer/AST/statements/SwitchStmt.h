#pragma once
#include <memory>

#include "AST/abstract/Statement.h"
#include "symbols/Var.h"

#include "CaseStmt.h"

class SwitchStmt : public Stmt
{
private:
    Var var;
    std::vector<std::unique_ptr<CaseStmt>> cases;
public:
    SwitchStmt(const Token& token, IFuncDeclStmt* funcDecl, Var var, std::vector<std::unique_ptr<CaseStmt>>& cases, ClassDeclStmt* classDecl = nullptr);
    void setVar(const Var& var);

    void analyze() const override;
    std::string translateToCpp() const override;
};
