#pragma once
#include "AST/abstract/Call.h"
#include "AST/abstract/Statement.h"

class HearStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Call>> calls;

public:
    HearStmt(const Token& token,  IFuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Call>>& calls, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
};
