#include "WhileStmt.h"

WhileStmt::WhileStmt(const Token& token, StmtWithBody stmt)
    : Stmt(token), stmt(std::move(stmt))
{
}

void WhileStmt::analyze() const
{
    stmt.body->analyze();
    stmt.expr->analyze();
}

std::string WhileStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() << "while (" << stmt.expr->translateToCpp() << ")" << std::endl << stmt.body->translateToCpp();
    return oss.str();
}