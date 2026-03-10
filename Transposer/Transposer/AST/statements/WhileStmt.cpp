#include "WhileStmt.h"

WhileStmt::WhileStmt(const Token& token, IFuncDeclStmt* funcDecl, StmtWithBody& stmt, ClassDeclStmt* classDecl)
    : Stmt(token, funcDecl, classDecl), stmt(std::move(stmt.expr), std::move(stmt.body))
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