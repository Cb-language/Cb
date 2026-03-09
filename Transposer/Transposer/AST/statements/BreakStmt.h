#pragma once
#include "AST/abstract/Statement.h"

class BreakStmt : public Stmt
{
private:

public:
    BreakStmt(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
};
