#include "WhileStmt.h"

WhileStmt::WhileStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, std::unique_ptr<Expr>& condition, std::unique_ptr<Stmt>& body)
    : Stmt(token, scope, funcDecl), condition(std::move(condition)), body(std::move(body))
{
}

void WhileStmt::analyze() const
{
    body->analyze();
    condition->analyze();
}

std::string WhileStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() << "while (" << condition->translateToCpp() << ")" << std::endl << body->translateToCpp();
    return oss.str();
}