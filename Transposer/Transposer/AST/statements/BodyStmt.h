#pragma once
#include "AST/abstract/Statement.h"

class BodyStmt : public Stmt
{
    std::vector<std::unique_ptr<Stmt>> stmts;
    bool isGlobal;
    bool hasBrace = true;
    bool isBreakable = false;
    bool isContinueAble = false;

public:
    BodyStmt(const Token& token, std::vector<std::unique_ptr<Stmt>>& stmts, bool isGlobal = false);
    std::vector<std::unique_ptr<Stmt>>& getStmts();
    const std::vector<std::unique_ptr<Stmt>>& getStmts() const;

    void analyze() const override;
    std::string translateToCpp() const override;

    void setHasBrace(bool hasBrace);
    void setBreakable(bool isBreakable);
    void setContinueAble(bool isContinueAble);
    void setSymbolTable(SymbolTable* symTable) const override;
};

