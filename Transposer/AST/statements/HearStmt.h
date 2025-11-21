#pragma once
#include "AST/abstract/Statement.h"
#include "expression/VarCallExpr.h"

class HearStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<VarCallExpr>> vars;

public:
    explicit HearStmt(const std::vector<std::unique_ptr<VarCallExpr>>& vars);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
