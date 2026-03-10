#pragma once
#include "BodyStmt.h"
#include "IfStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"

class CaseStmt : public Stmt
{
private:
    StmtWithBody stmt;
    bool isDefault = false;
public:
    CaseStmt(const Token& token, IFuncDeclStmt* funcDecl,
        StmtWithBody stmt, const bool isDefault = false, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;

};
