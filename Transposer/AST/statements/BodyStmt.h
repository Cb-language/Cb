#pragma once

#include "../Statements.h"

class BodyStmt
{
private:
    std::vector<std::unique_ptr<Stmt>> stmts;
public:
    const std::vector<std::unique_ptr<Stmt>>& getStmts() const;

};