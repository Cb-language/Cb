#pragma once
#include "AST/abstract/Call.h"
#include "AST/abstract/Statement.h"

class HearStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Call>> calls;

public:
    HearStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Call>>& calls);
    void analyze() const override;
    std::string translateToCpp() const override;
};
