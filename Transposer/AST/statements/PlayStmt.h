#pragma once

#include "AST/abstract/Statement.h"
#include "expression/ConstValueExpr.h"

class PlayStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<ConstValueExpr>> vars;

public:
    PlayStmt(const std::vector<std::unique_ptr<ConstValueExpr>>& vars);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
