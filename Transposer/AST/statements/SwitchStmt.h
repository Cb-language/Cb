#pragma once
#include <memory>

#include "AST/abstract/Statement.h"
#include "symbols/Var.h"
#include <sstream>

#include "CaseStmt.h"

class SwitchStmt : public Stmt
{
private:
    std::unique_ptr<Var> variable;
    std::vector<std::unique_ptr<CaseStmt>> cases;
    bool hasDefault = false;
public:
    SwitchStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Var> variable);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
