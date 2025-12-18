#include "WhileStmt.h"

WhileStmt::WhileStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Expr>& condition, std::unique_ptr<Stmt>& body) : Stmt(scope, funcDecl), condition(std::move(condition)), body(std::move(body))
{
}

bool WhileStmt::isLegal() const
{
    return body->isLegal() && condition->isLegal();
}

std::string WhileStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() << "while (" << condition->translateToCpp() << ")" << std::endl << body->translateToCpp();
    return oss.str();
}