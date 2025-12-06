#pragma once
#include "AST/abstract/Statement.h"


class BodyStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Stmt>> stmts;
    const bool isGlobal;
public:
    BodyStmt(Scope* scope, FuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Stmt>>& stmts, const bool isGlobal = false);
    std::vector<std::unique_ptr<Stmt>>& getStmts();

    bool isLegal() const override;
    std::string translateToCpp() const override;

};
