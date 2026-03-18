#pragma once
#include "AST/abstract/VarReference.h"
#include "AST/abstract/Statement.h"

class HearStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<VarReference>> references;

public:
    HearStmt(const Token& token, std::vector<std::unique_ptr<VarReference>>& refs);
    void analyze() const override;
    std::string translateToCpp() const override;
    void setSymbolTable(SymbolTable* symTable) const override;
};
