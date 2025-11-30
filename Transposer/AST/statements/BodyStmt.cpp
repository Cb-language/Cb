#include "BodyStmt.h"

const std::vector<std::unique_ptr<Stmt>>& BodyStmt::getStmts() const
{
    return stmts;
}