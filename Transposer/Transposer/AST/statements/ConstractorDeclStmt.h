#pragma once
#include "BodyStmt.h"
#include "AST/abstract/Statement.h"
#include "symbols/Constractor.h"

class ConstractorDeclStmt : public Stmt
{
private:
    std::unique_ptr<BodyStmt> body;
    Constractor constractor;

public:
    ConstractorDeclStmt(const Token& token, Scope* scope, const std::wstring& className, const std::vector<Var>& args);

    void setBody(std::unique_ptr<BodyStmt> body);

    void analyze() const override;
    std::string translateToCpp() const override;
    inline std::string translateToH() const override;
};
