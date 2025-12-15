#pragma once
#include "BodyStmt.h"
#include "FuncDeclStmt.h"
#include "AST/abstract/Statement.h"

class ElseStmt : public Stmt
{
private:
    std::unique_ptr<BodyStmt> body;
public:
    ElseStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<BodyStmt>& body);
    bool isLegal() const override;
    std::string translateToCpp() const override;
};
