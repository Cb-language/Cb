#pragma once
#include <memory>

#include "AST/abstract/Statement.h"
#include "symbols/Var.h"

#include "CaseStmt.h"

class SwitchStmt : public Stmt
{
private:
    const Var var;
    std::vector<std::unique_ptr<CaseStmt>> cases;
public:
    SwitchStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, Var var, std::vector<std::unique_ptr<CaseStmt>>& cases);
    void analyze() const override;
    std::string translateToCpp() const override;
};
