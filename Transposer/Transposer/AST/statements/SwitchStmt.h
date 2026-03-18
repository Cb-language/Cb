#pragma once
#include <memory>

#include "AST/abstract/Statement.h"
#include "symbols/Var.h"

#include "CaseStmt.h"

class SwitchStmt : public Stmt
{
private:
    std::unique_ptr<Expr> expr;
    std::vector<std::unique_ptr<CaseStmt>> cases;
public:
    SwitchStmt(const Token& token, std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<CaseStmt>>& cases);
    void setExpr(std::unique_ptr<Expr> expr);

    void analyze() const override;
    std::string translateToCpp() const override;
    void setSymbolTable(SymbolTable* symTable) const override;
};
