#pragma once
#include "IfStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class WhileStmt : public Stmt
{
private:;
    StmtWithBody stmt;
public:
    WhileStmt(const Token& token, IFuncDeclStmt* funcDecl, StmtWithBody& stmt, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
};