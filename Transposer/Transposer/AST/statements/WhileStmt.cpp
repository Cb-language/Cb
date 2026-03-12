#include "WhileStmt.h"

WhileStmt::WhileStmt(const Token& token, StmtWithBody stmt)
    : Stmt(token), stmt(std::move(stmt))
{
}

void WhileStmt::analyze() const
{
    if (symTable == nullptr) return;

    stmt.expr->setSymbolTable(symTable);
    stmt.expr->setScope(scope);
    stmt.expr->setClassNode(currClass);
    stmt.expr->analyze();

    if (auto* body = dynamic_cast<BodyStmt*>(stmt.body.get()))
    {
        body->setBreakable(true);
        body->setContinueAble(true);
    }

    stmt.body->setSymbolTable(symTable);
    stmt.body->setScope(scope);
    stmt.body->setClassNode(currClass);
    stmt.body->analyze();
}

std::string WhileStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() << "while (" << stmt.expr->translateToCpp() << ")" << std::endl << stmt.body->translateToCpp();
    return oss.str();
}