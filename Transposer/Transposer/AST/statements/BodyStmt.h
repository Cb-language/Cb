#pragma once
#include "AST/abstract/Statement.h"

class BodyStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Stmt>> stmts;
    const bool isGlobal;
    bool hasBrace = true;
public:
    BodyStmt(const Token& token, IFuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Stmt>>& stmts, const bool isGlobal = false, ClassDeclStmt* classDecl = nullptr);
    std::vector<std::unique_ptr<Stmt>>& getStmts();
    const std::vector<std::unique_ptr<Stmt>>& getStmts() const; // Added const overload

    void analyze() const override;
    std::string translateToCpp() const override;

    void setHasBrace(const bool hasBrace);
};
