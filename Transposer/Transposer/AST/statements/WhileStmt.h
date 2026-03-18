#pragma once
#include "IfStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class WhileStmt : public Stmt
{
private:;
    StmtWithBody stmt;
public:
    WhileStmt(const Token& token, StmtWithBody stmt);
    void analyze() const override;
    std::string translateToCpp() const override;
    void setSymbolTable(SymbolTable* symTable) const override;
};