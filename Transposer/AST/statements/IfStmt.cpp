#include "IfStmt.h"

IfStmt::IfStmt(Scope *scope, FuncDeclStmt *funcDecl, const Expr *expr, BodyStmt* body) : Stmt(scope, funcDecl), expr(expr), body(body)
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

const Expr *IfStmt::getExpression() const
{
    return expr;
}
