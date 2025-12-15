#include "ElseIfStmt.h"

ElseIfStmt::ElseIfStmt(Scope *scope, FuncDeclStmt *func_decl, std::unique_ptr<Expr> &expr,
    std::unique_ptr<BodyStmt> &body) : Stmt(scope, func_decl), expr(std::move(expr)), body(std::move(body))
{
}

bool ElseIfStmt::isLegal() const
{
    return expr->isLegal() && body->isLegal();
}

std::string ElseIfStmt::translateToCpp() const
{
    return getTabs() + "else if (" + expr->translateToCpp() + ")\n" + body->translateToCpp();
}
