#pragma once
#include "AST/abstract/Statement.h"

class HearStmt : public Stmt
{
private:
    const std::vector<Var> vars;

public:
    explicit HearStmt(const std::vector<Var>& vars);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
