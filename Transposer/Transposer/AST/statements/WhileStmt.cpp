#include "WhileStmt.h"

WhileStmt::WhileStmt(const Token& token, IFuncDeclStmt* funcDecl, std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body, ClassDeclStmt* classDecl)
    : Stmt(token, funcDecl, classDecl), condition(std::move(condition)), body(std::move(body))
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