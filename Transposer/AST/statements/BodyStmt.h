#pragma once
#include "AST/abstract/Statement.h"

class BodyStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Stmt>> stmts;
    const bool isGlobal;
    bool hasBrace = true;
public:
    BodyStmt(Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Stmt>>& stmts, const bool isGlobal = false);
    std::vector<std::unique_ptr<Stmt>>& getStmts();

    void analyze() const override;
    std::string translateToCpp() const override;

    void setHasBrace(const bool hasBrace);
};
