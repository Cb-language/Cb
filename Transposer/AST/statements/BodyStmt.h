#pragma once

#include "../Statements.h"

class BodyStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Stmt>> stmts;
public:
    BodyStmt(const std::vector<std::unique_ptr<Stmt>>& stmts);
    const std::vector<std::unique_ptr<Stmt>>& getStmts() const;

    bool isLegal() const override;
    std::string translateToCpp() override;

};