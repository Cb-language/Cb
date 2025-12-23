#pragma once
#include <memory>

#include "AST/abstract/Statement.h"
#include "symbols/Var.h"
#include <sstream>

#include "CaseStmt.h"

class SwitchStmt : public Stmt
{
private:
    const Var var;
    std::vector<std::unique_ptr<CaseStmt>> cases;
public:
    SwitchStmt(Scope* scope, FuncDeclStmt* funcDecl, Var var, std::vector<std::unique_ptr<CaseStmt>>& cases);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
