#include "IfStmt.h"

IfStmt::IfStmt(Scope *scope, FuncDeclStmt *funcDecl, std::unique_ptr<Expr> &expr, std::unique_ptr<BodyStmt> &body) : Stmt(scope, funcDecl), expr(std::move(expr)), body(std::move(body))
{
}

bool IfStmt::isLegal() const
{
    return body->isLegal() && expr->isLegal();
}

std::string IfStmt::translateToCpp() const
{
    return getTabs() + "if (" + expr->translateToCpp() + ")\n" + body->translateToCpp();
}