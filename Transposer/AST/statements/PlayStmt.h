#pragma once

#include "AST/abstract/Statement.h"
#include "expression/VarCallExpr.h"

class PlayStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<VarCallExpr>> vars;

public:
    PlayStmt(const std::vector<std::unique_ptr<VarCallExpr>>& vars);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};