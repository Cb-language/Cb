#pragma once
#include "AST/abstract/Statement.h"

class ContinueStmt : public Stmt
{
private:

public:
    ContinueStmt(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
};
